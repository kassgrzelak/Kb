//
// Created by kassie on 09/01/2026.
//

#include "Chunk.hpp"

size_t Chunk::size() const noexcept
{
	return code.size();
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
