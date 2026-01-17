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

	Compiler(const char* source, Chunk& chunk);

	bool compile();

private:
	Scanner scanner;
	Parser parser;
	Chunk& chunk;

	static const ParseRule rules[];

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

	void parsePrecedence(Precedence precedence);
	[[nodiscard]] static const ParseRule& getRule(TokenType type) ;

	void endCompiler() const;
};

#endif //KFLAT_COMPILER_HPP