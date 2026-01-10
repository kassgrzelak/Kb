//
// Created by kassie on 09/01/2026.
//

#ifndef KFLAT_CHUNK_HPP
#define KFLAT_CHUNK_HPP

#include <vector>

#include "common.hpp"

enum OpCode
{
	OP_RETURN
};

struct Chunk
{
	std::vector<uint8_t> code;

	void write(OpCode op) noexcept;
};

#endif //KFLAT_CHUNK_HPP