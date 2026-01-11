//
// Created by kassie on 10/01/2026.
//

#include "debug.hpp"

#include <cstdio>
#include <memory>

#include "Value.hpp"

void disassembleChunk(const Chunk &chunk, const char *name)
{
	printf("=== %s ===\n", name);

	for (size_t offset = 0; offset < chunk.size();)
		offset = disassembleInstruction(chunk, offset);
}

static size_t simpleInstruction(const char* name, const size_t offset)
{
	printf("%s\n", name);
	return offset + 1;
}

static size_t constantInstruction(const char* name, const Chunk& chunk, const size_t offset)
{
	const uint8_t constantIndex = chunk.code[offset + 1];

	printf("%-16s %4d '", name, constantIndex);
	printValue(chunk.constants[constantIndex]);
	printf("'\n");

	return offset + 2;
}

static size_t constantInstruction24(const char* name, const Chunk& chunk, const size_t offset)
{
	const uint8_t byte1 = chunk.code[offset + 1];
	const uint8_t byte2 = chunk.code[offset + 2];
	const uint8_t byte3 = chunk.code[offset + 3];

	const size_t constantIndex = (byte1 << 16) | (byte2 << 8) | byte3;

	printf("%-16s %4lu '", name, constantIndex);
	printValue(chunk.constants[constantIndex]);
	printf("'\n");

	return offset + 4;
}

size_t disassembleInstruction(const Chunk &chunk, const size_t offset)
{
	printf("0x%04x ", static_cast<unsigned int>(offset));

	if (offset > 0 && chunk.getLine(offset) == chunk.getLine(offset - 1))
		printf("   | ");
	else
		printf("%4lu ", chunk.getLine(offset));

	switch (const uint8_t instruction = chunk.code[offset])
	{
		case OP_CONSTANT:
			return constantInstruction("OP_CONSTANT", chunk, offset);
		case OP_CONSTANT_24:
			return constantInstruction24("OP_CONSTANT_24", chunk, offset);
		case OP_RETURN:
			return simpleInstruction("OP_RETURN", offset);

		default:
			printf("UNK%d", instruction);
			return offset + 1;
	}
}
