//
// Created by kassie on 11/01/2026.
//

#ifndef KFLAT_VM_HPP
#define KFLAT_VM_HPP
#include "Chunk.hpp"

enum InterpretResult : uint8_t
{
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
	INTERPRET_UNKNOWN_OPCODE
};

class VM
{
public:
	VM() noexcept;

	InterpretResult interpret(const char* source);

private:
	const Chunk* chunk = nullptr;
	std::vector<Value> stack;

	InterpretResult run();

	void push(const Value& value) noexcept;
	Value pop() noexcept;
};

#endif //KFLAT_VM_HPP