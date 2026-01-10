//
// Created by kassie on 10/01/2026.
//

#ifndef KFLAT_DEBUG_HPP
#define KFLAT_DEBUG_HPP
#include "Chunk.hpp"

void disassembleChunk(const Chunk& chunk, const char* name);
int disassembleInstruction(const Chunk& chunk, int offset);

#endif //KFLAT_DEBUG_HPP