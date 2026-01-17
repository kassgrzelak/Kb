//
// Created by kassie on 09/01/2026.
//

#include "../include/Chunk.hpp"

#define UINT24_MAX 0xffffff

size_t Chunk::size() const noexcept
{
	return code.size();
}

const uint8_t* Chunk::getCodePointer() const noexcept
{
	return code.data();
}

size_t Chunk::getLine(size_t byteIndex) const
{
	for (size_t i = 1; i < lineData.size(); i += 2)
	{
		if (lineData[i] > byteIndex)
			return lineData[i - 1];

		byteIndex -= lineData[i];
	}

	return -1;
}

Value Chunk::getConstant(const size_t constantIndex) const noexcept
{
	return constants[constantIndex];
}

void Chunk::write(const OpCode op, const size_t line) noexcept
{
	code.push_back(op);
	addLineData(line);
}

void Chunk::write(const uint8_t byte, const size_t line) noexcept
{
	code.push_back(byte);
	addLineData(line);
}

bool Chunk::writeConstant(const Value value, const size_t line) noexcept
{
	const size_t constantIndex = addConstant(value);

	if (constantIndex <= UINT8_MAX)
	{
		write(OP_CONSTANT_8, line);
		write(static_cast<uint8_t>(constantIndex), line);
	}
	else if (constantIndex <= UINT16_MAX)
	{
		write(OP_CONSTANT_16, line);

		const uint8_t byte1 = constantIndex >> 8;
		const uint8_t byte2 = constantIndex;

		write(byte1, line);
		write(byte2, line);
	}
	else if (constantIndex <= UINT24_MAX)
	{
		write(OP_CONSTANT_24, line);

		const uint8_t byte1 = constantIndex >> 16;
		const uint8_t byte2 = constantIndex >> 8;
		const uint8_t byte3 = constantIndex;

		write(byte1, line);
		write(byte2, line);
		write(byte3, line);
	}
	else if (constantIndex <= UINT32_MAX)
	{
		write(OP_CONSTANT_32, line);

		const uint8_t byte1 = constantIndex >> 24;
		const uint8_t byte2 = constantIndex >> 16;
		const uint8_t byte3 = constantIndex >> 8;
		const uint8_t byte4 = constantIndex;

		write(byte1, line);
		write(byte2, line);
		write(byte3, line);
		write(byte4, line);
	}
	else return false; // If you're using more than 2^32 constants in a chunk something is severely wrong with you.

	return true;
}

size_t Chunk::addConstant(const Value value) noexcept
{
	constants.push_back(value);
	return constants.size() - 1;
}

void Chunk::addLineData(const size_t line) noexcept
{
	if (lineData.empty())
	{
		lineData.push_back(line);
		lineData.push_back(1);
		return;
	}

	if (line == lineData[lineData.size() - 2])
		lineData[lineData.size() - 1] += 1;
	else
	{
		lineData.push_back(line);
		lineData.push_back(1);
	}
}

#undef UINT24_MAX