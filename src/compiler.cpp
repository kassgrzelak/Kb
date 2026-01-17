//
// Created by kassie on 13/01/2026.
//

#include "compiler.hpp"

#include <cstdio>
#include <cstdlib>

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

void Compiler::advance()
{
	parser.previous = parser.current;

	for (;;)
	{
		parser.current = scanToken();
		if (parser.current.type != TOKEN_ERROR) break;

		errorAtCurrent(parser.current.start);
	}
}

void Compiler::consume(TokenType type, const char *message)
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
	TokenType operatorType = parser.previous.type;
	ParseRule* rule = getRule(operatorType);
	parsePrecedence(static_cast<Precedence>(rule->precedence + 1));

	switch (operatorType)
	{
		case TOKEN_PLUS: emitByte(OP_ADD); break;
		case TOKEN_MINUS: emitByte(OP_SUBTRACT); break;
		case TOKEN_STAR: emitByte(OP_MULTIPLY); break;
		case TOKEN_SLASH: emitByte(OP_DIVIDE); break;

		default: return; // Unreachable.
	}
}

void Compiler::parsePrecedence(Precedence prec)
{

}

void Compiler::endCompiler() const
{
	emitByte(OP_RETURN);
}
