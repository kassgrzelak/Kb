//
// Created by kassie on 11/01/2026.
//

#include "VM.hpp"

#include <cstdio>

#include "debug.hpp"

VM::VM() noexcept
{

}

InterpretResult VM::interpret(const Chunk* chunk)
{
	this->chunk = chunk;
	return run();
}



InterpretResult VM::run()
{
	const uint8_t* ip = chunk->getCodePointer();

#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (chunk->getConstant(READ_BYTE()))

	for (;;)
	{
#ifdef DEBUG_TRACE_EXECUTION
		disassembleInstruction(*chunk, ip - chunk->getCodePointer());
#endif // DEBUG_TRACE_EXECUTION
		printf("          ");
		for (const auto& slot : stack)
		{
			printf("[ ");
			printValue(slot);
			printf(" ]");
		}
		printf("\n");
		switch (uint8_t instruction = READ_BYTE())
		{
			case OP_CONSTANT:
			{
				const Value constant = READ_CONSTANT();
				push(constant);
				break;
			}

			case OP_RETURN:
			{
				printValue(pop());
				printf("\n");
				return INTERPRET_OK;
			}

			default:
				return INTERPRET_UNKNOWN_OPCODE;
		}
	}

#undef READ_BYTE
#undef READ_CONSTANT
}

void VM::push(const Value& value) noexcept
{
	stack.push_back(value);
}

Value VM::pop() noexcept
{
	const Value value = stack.back();
	stack.pop_back();
	return value;
}
