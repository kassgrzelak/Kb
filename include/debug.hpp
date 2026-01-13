//
// Created by kassie on 10/01/2026.
//

#ifndef KFLAT_DEBUG_HPP
#define KFLAT_DEBUG_HPP
#include "Chunk.hpp"

void disassembleChunk(const Chunk& chunk, const char* name);
size_t disassembleInstruction(const Chunk& chunk, size_t offset);

#endif //KFLAT_DEBUG_HPP