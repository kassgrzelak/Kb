//
// Created by kassie on 11/01/2026.
//

#include "../include/VM.hpp"

#include <complex>
#include <cstdio>

#include "compiler.hpp"
#include "debug.hpp"

InterpretResult VM::interpret(const char *source)
{
	chunk = new Chunk;
	Compiler compiler{ source, *chunk };

	if (!compiler.compile())
	{
		delete chunk;
		return INTERPRET_COMPILE_ERROR;
	}

	const InterpretResult result = run();

	delete chunk;
	return result;
}

static size_t readByte16(const uint8_t* ip)
{
	const uint8_t byte1 = *ip++ << 8;
	const uint8_t byte2 = *ip++;

	return byte1 | byte2;
}

static size_t readByte24(const uint8_t* ip)
{
	const uint8_t byte1 = *ip++ << 16;
	const uint8_t byte2 = *ip++ << 8;
	const uint8_t byte3 = *ip++;

	return byte1 | byte2 | byte3;
}

static size_t readByte32(const uint8_t* ip)
{
	const uint8_t byte1 = *ip++ << 24;
	const uint8_t byte2 = *ip++ << 16;
	const uint8_t byte3 = *ip++ << 8;
	const uint8_t byte4 = *ip++;

	return byte1 | byte2 | byte3 | byte4;
}

InterpretResult VM::run()
{
	const uint8_t* ip = chunk->getCodePointer();

#define READ_BYTE() (*ip++)
#define READ_BYTE_16() (readByte16(ip))
#define READ_BYTE_24() (readByte24(ip))
#define READ_BYTE_32() (readByte32(ip))
#define READ_CONSTANT_8() (chunk->getConstant(READ_BYTE()))
#define READ_CONSTANT_16() (chunk->getConstant(READ_BYTE_16()))
#define READ_CONSTANT_24() (chunk->getConstant(READ_BYTE_24()))
#define READ_CONSTANT_32() (chunk->getConstant(READ_BYTE_32()))
#define BINARY_OP(op) \
	do \
	{ \
		const double b = pop(); \
		const double a = pop(); \
		push(a op b); \
	} \
	while (false)

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
		switch (READ_BYTE())
		{
			case OP_CONSTANT_8:
			{
				const Value constant = READ_CONSTANT_8();
				push(constant);
				break;
			}
			case OP_CONSTANT_16:
			{
				const Value constant = READ_CONSTANT_16();
				push(constant);
				break;
			}
			case OP_CONSTANT_24:
			{
				const Value constant = READ_CONSTANT_24();
				push(constant);
				break;
			}
			case OP_CONSTANT_32:
			{
				const Value constant = READ_CONSTANT_32();
				push(constant);
				break;
			}

			case OP_NEGATE: stack.back() *= -1; break;

			case OP_ADD: BINARY_OP(+); break;
			case OP_SUBTRACT: BINARY_OP(-); break;
			case OP_MULTIPLY: BINARY_OP(*); break;
			case OP_DIVIDE: BINARY_OP(/); break;
			case OP_EXPONENT:
			{
				const double y = pop();
				const double x = pop();
				push(std::pow(x, y));
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
#undef READ_BYTE_16
#undef READ_BYTE_24
#undef READ_BYTE_32
#undef READ_CONSTANT_8
#undef READ_CONSTANT_16
#undef READ_CONSTANT_24
#undef READ_CONSTANT_32
#undef BINARY_OP
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
