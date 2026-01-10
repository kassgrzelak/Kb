#include "Chunk.hpp"
#include "debug.hpp"

int main(int argc, const char* argv[])
{
	Chunk chunk;

	const size_t constant = chunk.addConstant(1.2);
	chunk.write(OP_CONSTANT, 1);
	chunk.write(static_cast<uint8_t>(constant), 1);

	chunk.write(OP_RETURN, 2);

	disassembleChunk(chunk, "test chunk");

	return 0;
}
