//
// Created by kassie on 10/01/2026.
//

#include "debug.hpp"

#include <cstdio>

#include "Value.hpp"

void disassembleChunk(const Chunk &chunk, const char *name)
{
	printf("=== %s ===\n", name);

	for (int offset = 0; offset < chunk.size();)
		offset = disassembleInstruction(chunk, offset);
}

static int simpleInstruction(const char* name, const int offset)
{
	printf("%s\n", name);
	return offset + 1;
}

static int constantInstruction(const char* name, const Chunk& chunk, const int offset)
{
	const uint8_t constantIndex = chunk.code[offset + 1];

	printf("%-16s %4d '", name, constantIndex);
	printValue(chunk.constants[constantIndex]);
	printf("'\n");

	return offset + 2;
}

int disassembleInstruction(const Chunk &chunk, const int offset)
{
	printf("%04d ", offset);

	if (offset > 0 && chunk.getLine(offset) == chunk.getLine(offset - 1))
		printf("   | ");
	else
		printf("%4d ", chunk.getLine(offset));

	switch (const uint8_t instruction = chunk.code[offset])
	{
		case OP_CONSTANT:
			return constantInstruction("OP_CONSTANT", chunk, offset);
		case OP_RETURN:
			return simpleInstruction("OP_RETURN", offset);

		default:
			printf("UNK%d", instruction);
			return offset + 1;
	}
}
