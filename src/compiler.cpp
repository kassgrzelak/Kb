//
// Created by kassie on 13/01/2026.
//

#include "compiler.hpp"

#include <cstdio>
#include <cstdlib>

#include "debug.hpp"
#include "Scanner.hpp"

Compiler::Compiler(const char *source, Chunk &chunk) : scanner(source), chunk(chunk) { }

bool Compiler::compile()
{
	advance();
	expression();
	consume(TOKEN_EOF, "Expect end of expression.");

	endCompiler();
	return !parser.hadError;
}

const Compiler::ParseRule Compiler::rules[] = {
    /* TOKEN_LEFT_PAREN */   { &Compiler::grouping, nullptr,              PREC_NONE },
    /* TOKEN_RIGHT_PAREN */  { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_LEFT_BRACE */   { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_RIGHT_BRACE */  { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_COMMA */        { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_DOT */          { nullptr,             nullptr,              PREC_NONE },

    /* TOKEN_INC */          { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_DEC */          { nullptr,             nullptr,              PREC_NONE },

    /* TOKEN_MINUS */        { &Compiler::unary,    &Compiler::binary,    PREC_TERM },
    /* TOKEN_PLUS */         { nullptr,             &Compiler::binary,    PREC_TERM },
    /* TOKEN_SLASH */        { nullptr,             &Compiler::binary,    PREC_FACTOR },
    /* TOKEN_STAR */         { nullptr,             &Compiler::binary,    PREC_FACTOR },
    /* TOKEN_CARET */        { nullptr,             &Compiler::binary,    PREC_EXPONENT },

    /* TOKEN_SEMICOLON */    { nullptr,             nullptr,              PREC_NONE },

    /* TOKEN_BANG */         { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_BANG_EQUAL */   { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_EQUAL */        { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_EQUAL_EQUAL */  { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_GREATER */      { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_GREATER_EQUAL */{ nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_LESS */         { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_LESS_EQUAL */   { nullptr,             nullptr,              PREC_NONE },

    /* TOKEN_IDENTIFIER */   { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_STRING */       { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_NUMBER */       { &Compiler::number,   nullptr,              PREC_NONE },

    /* TOKEN_IF */           { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_ELSE */         { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_AND */          { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_OR */           { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_TRUE */         { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_FALSE */        { nullptr,             nullptr,              PREC_NONE },

    /* TOKEN_CLASS */        { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_SUPER */        { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_THIS */         { nullptr,             nullptr,              PREC_NONE },

    /* TOKEN_FOR */          { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_WHILE */        { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_FUNC */         { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_PRINT */        { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_RETURN */       { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_VAR */          { nullptr,             nullptr,              PREC_NONE },

    /* TOKEN_ZILCH */        { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_INF */          { nullptr,             nullptr,              PREC_NONE },

    /* TOKEN_ERROR */        { nullptr,             nullptr,              PREC_NONE },
    /* TOKEN_EOF */          { nullptr,             nullptr,              PREC_NONE },
};

void Compiler::advance()
{
	parser.previous = parser.current;

	for (;;)
	{
		parser.current = scanner.scanToken();
		if (parser.current.type != TOKEN_ERROR) break;

		errorAtCurrent(parser.current.start);
	}
}

void Compiler::consume(const TokenType type, const char *message)
{
	if (parser.current.type == type)
	{
		advance();
		return;
	}

	errorAtCurrent(message);
}

void Compiler::emitByte(const OpCode op) const
{
	chunk.write(op, parser.previous.line);
}

void Compiler::emitByte(const uint8_t byte) const
{
	chunk.write(byte, parser.previous.line);
}

void Compiler::emitOpBytePair(const OpCode op, const uint8_t byte) const
{
	emitByte(op);
	emitByte(byte);
}

void Compiler::emitConstant(const Value value)
{
	if (!chunk.writeConstant(value, parser.previous.line))
		error("Too many constants in one chunk (limit is 2^32).");
}

void Compiler::errorAtCurrent(const char* message)
{
	errorAt(parser.current, message);
}

void Compiler::error(const char* message)
{
	errorAt(parser.previous, message);
}

void Compiler::errorAt(const Token& token, const char *message)
{
	if (parser.panicMode) return;
	parser.panicMode = true;

	fprintf(stderr, "[line %lu] Error", token.line);

	if (token.type == TOKEN_EOF)
		fprintf(stderr, " at end");
	else if (token.type == TOKEN_ERROR)
	{
		// Nothing.
	}
	else
		fprintf(stderr, " at '%.*s'", static_cast<int>(token.length), token.start);

	fprintf(stderr, ": %s\n", message);
	parser.hadError = true;
}

void Compiler::expression()
{
	parsePrecedence(PREC_ASSIGNMENT);
}

void Compiler::grouping()
{
	expression();
	consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void Compiler::number()
{
	const double value = strtod(parser.previous.start, nullptr);
	emitConstant(value);
}

void Compiler::unary()
{
	const TokenType operatorType = parser.previous.type;

	parsePrecedence(PREC_UNARY);

	switch (operatorType)
	{
		case TOKEN_MINUS: emitByte(OP_NEGATE); break;

		default: return; // Unreachable.
	}
}

void Compiler::binary()
{
	const TokenType operatorType = parser.previous.type;
	const auto rule = getRule(operatorType);
	parsePrecedence(static_cast<Precedence>(rule.precedence + 1));

	switch (operatorType)
	{
		case TOKEN_PLUS: emitByte(OP_ADD); break;
		case TOKEN_MINUS: emitByte(OP_SUBTRACT); break;
		case TOKEN_STAR: emitByte(OP_MULTIPLY); break;
		case TOKEN_SLASH: emitByte(OP_DIVIDE); break;
		case TOKEN_CARET: emitByte(OP_EXPONENT); break;

		default: return; // Unreachable.
	}
}

void Compiler::parsePrecedence(const Precedence precedence)
{
	advance();
	const ParseFunc prefixRule = getRule(parser.previous.type).prefix;

	if (prefixRule == nullptr)
	{
		error("Expect expression.");
		return;
	}

	(this->*prefixRule)();

	while (precedence <= getRule(parser.current.type).precedence)
	{
		advance();
		const ParseFunc infixRule = getRule(parser.previous.type).infix;
		(this->*infixRule)();
	}
}

const Compiler::ParseRule& Compiler::getRule(const TokenType type)
{
	return rules[type];
}

void Compiler::endCompiler() const
{
	emitByte(OP_RETURN);

#ifdef DEBUG_PRINT_CODE
	if (!parser.hadError)
		disassembleChunk(chunk, "code");
#endif
}
