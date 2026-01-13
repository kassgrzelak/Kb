//
// Created by kassie on 13/01/2026.
//

#include "../include/compiler.hpp"

#include <cstdio>

#include "../include/scanner.hpp"

void compile(VM& vm, const char* source)
{
	initScanner(source);
	size_t line = 0;

	for (;;)
	{
		Token token = scanToken();

		if (token.line != line)
		{
			printf("%4lu ", token.line);
			line = token.line;
		}
		else
		{
			printf("   | ");
		}

		printf("%2d '%.*s'\n", token.type, static_cast<int>(token.length), token.start);

		if (token.type == TOKEN_EOF) break;
	}
}
