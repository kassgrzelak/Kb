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
	OP_CONSTANT,
	OP_RETURN
};

struct Chunk
{
	std::vector<uint8_t> code;
	std::vector<Value> constants;

	[[nodiscard]] size_t size() const noexcept;
	[[nodiscard]] int getLine(int byteIndex) const;

	void write(OpCode op, int line) noexcept;
	void write(uint8_t byte, int line) noexcept;

	size_t addConstant(Value value) noexcept;

private:
	// Run length encoded (line_num, num_bytes).
	std::vector<int> lineData;

	void addLineData(int line) noexcept;
};

#endif //KFLAT_CHUNK_HPP