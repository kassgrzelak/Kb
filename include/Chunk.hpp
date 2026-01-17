//
// Created by kassie on 09/01/2026.
//

#ifndef KFLAT_CHUNK_HPP
#define KFLAT_CHUNK_HPP

#include <vector>

#include "common.hpp"
#include "Value.hpp"

enum OpCode : uint8_t
{
	OP_CONSTANT_8, OP_CONSTANT_16, OP_CONSTANT_24, OP_CONSTANT_32,

	OP_NEGATE,

	OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE, OP_EXPONENT,

	OP_RETURN
};

struct Chunk
{
	std::vector<uint8_t> code;
	std::vector<Value> constants;

	[[nodiscard]] size_t size() const noexcept;
	[[nodiscard]] const uint8_t* getCodePointer() const noexcept;
	[[nodiscard]] size_t getLine(size_t byteIndex) const;
	[[nodiscard]] Value getConstant(size_t constantIndex) const noexcept;

	void write(OpCode op, size_t line) noexcept;
	void write(uint8_t byte, size_t line) noexcept;
	// Returns true if success, returns false if more than 2^32 constants in one chunk and therefore failure.
	bool writeConstant(Value value, size_t line) noexcept;

	size_t addConstant(Value value) noexcept;

private:
	// Run length encoded (line_num, num_bytes).
	std::vector<size_t> lineData;

	void addLineData(size_t line) noexcept;
};

#endif //KFLAT_CHUNK_HPP