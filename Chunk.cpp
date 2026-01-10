//
// Created by kassie on 09/01/2026.
//

#include "Chunk.hpp"

void Chunk::write(const OpCode op) noexcept
{
	code.push_back(op);
}
