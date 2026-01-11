#include "Chunk.hpp"
#include "debug.hpp"
#include "VM.hpp"

int main(int argc, const char* argv[])
{
	VM vm{};

	Chunk chunk;

	// for (int i = 0; i < 259; ++i)
	// {
	// 	chunk.writeConstant(1.2, 1);
	// }

	chunk.writeConstant(4, 1);
	chunk.writeConstant(1, 1);
	chunk.write(OP_SUBTRACT, 1);
	chunk.writeConstant(2, 1);
	chunk.write(OP_MULTIPLY, 1);

	chunk.write(OP_RETURN, 2);

	vm.interpret(&chunk);

	return 0;
}
