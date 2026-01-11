#include "Chunk.hpp"
#include "debug.hpp"
#include "VM.hpp"

int main(int argc, const char* argv[])
{
	VM vm{};

	Chunk chunk;

	chunk.writeConstant(1.2, 1);
	chunk.write(OP_RETURN, 1);

	vm.interpret(&chunk);

	return 0;
}
