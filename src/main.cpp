#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <limits>

#include "../include/VM.hpp"

static void repl(VM& vm)
{
	char line[1024];

	for (;;)
	{
		printf("> ");

		if (!fgets(line, sizeof(line), stdin))
		{
			printf("\n");
			break;
		}

		vm.interpret(line);
	}
}

static void runFile(VM& vm, const char* path)
{
	FILE* file = fopen(path, "rb");
	if (file == nullptr)
	{
		fprintf(stderr, "Couldn't open file '%s'.\n", path);
		exit(74);
	}

	fseek(file, 0L, SEEK_END);
	const size_t fileSize = ftell(file);
	rewind(file);

	char* source = static_cast<char *>(malloc(fileSize + 1));
	if (source == nullptr)
	{
		fprintf(stderr, "Not enough memory to read '%s'.\n", path);
		exit(74);
	}

	const size_t bytesRead = fread(source, sizeof(char), fileSize, file);
	if (bytesRead < fileSize)
	{
		fprintf(stderr, "Couldn't read file '%s'.\n", path);
		exit(74);
	}

	source[bytesRead] = '\0';

	fclose(file);

	const InterpretResult result = vm.interpret(source);
	free(source);

	if (result == INTERPRET_COMPILE_ERROR) exit(65);
	if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(const int argc, const char* argv[])
{
	VM vm;

	if (argc == 1)
		repl(vm);
	else if (argc == 2)
		runFile(vm, argv[1]);
	else
	{
		fprintf(stderr, "Usage: kflat [path]\n");
		exit(64);
	}

	return 0;
}
