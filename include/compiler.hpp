//
// Created by kassie on 13/01/2026.
//

#ifndef KFLAT_COMPILER_HPP
#define KFLAT_COMPILER_HPP
#include <cstdint>

#include "Chunk.hpp"
#include "Scanner.hpp"

struct Chunk;

struct Parser
{
	Token current;
	Token previous;
	bool hadError = false;
	bool panicMode = false;
};

enum Precedence
{
	PREC_NONE,
	PREC_ASSIGNMENT,  // =
	PREC_OR,          // or
	PREC_AND,         // and
	PREC_EQUALITY,    // == !=
	PREC_COMPARISON,  // < > <= >=
	PREC_TERM,        // + -
	PREC_FACTOR,      // * /
	PREC_EXPONENT,    // ^
	PREC_UNARY,       // ! -
	PREC_CALL,        // . ()
	PREC_PRIMARY
};

class Compiler
{
public:
	using ParseFunc = void (Compiler::*)();

	struct ParseRule
	{
		ParseFunc prefix;
		ParseFunc infix;
		Precedence precedence;
	};

	static const ParseRule rules[];

	Compiler(const char* source, Chunk& chunk);

	bool compile();

private:
	Scanner scanner;
	Parser parser;
	Chunk& chunk;

	void advance();
	void consume(TokenType type, const char* message);

	void emitByte(OpCode op) const;
	void emitByte(uint8_t byte) const;
	void emitOpBytePair(OpCode op, uint8_t byte) const;
	void emitConstant(Value value);

	void errorAtCurrent(const char* message);
	void error(const char* message);
	void errorAt(const Token& token, const char* message);

	void expression();
	void grouping();
	void number();
	void unary();
	void binary();

	void parsePrecedence(Precedence prec);

	void endCompiler() const;
};

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

#endif //KFLAT_COMPILER_HPP