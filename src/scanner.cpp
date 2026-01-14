//
// Created by kassie on 13/01/2026.
//

#include "../include/scanner.hpp"

#include <cstring>

struct Scanner
{
	const char* start;
	const char* current;
	int line;
};

Scanner scanner{};

void initScanner(const char* source)
{
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;
}

static bool isAlpha(const char c) {
	return (c >= 'a' && c <= 'z') ||
		   (c >= 'A' && c <= 'Z') ||
			c == '_';
}

static bool isDigit(const char c)
{
	return c >= '0' && c <= '9';
}

static bool atEnd()
{
	return *scanner.current == '\0';
}

static char advance()
{
	++scanner.current;
	return scanner.current[-1];
}

static char peek()
{
	return *scanner.current;
}

static char peekNext()
{
	if (atEnd()) return '\0';
	return scanner.current[1];
}

// Check if next character is equal to expected, if not return false, if so advance current and return true.
static bool match(const char expected)
{
	if (atEnd())
		return false;
	if (*scanner.current != expected)
		return false;

	scanner.current++;
	return true;
}


static Token makeToken(const TokenType type)
{
	Token token;

	token.type = type;
	token.start = scanner.start;
	token.length = static_cast<size_t>(scanner.current - scanner.start);
	token.line = scanner.line;

	return token;
}

static Token errorToken(const char* message) {
	Token token;

	token.type = TOKEN_ERROR;
	token.start = message;
	token.length = static_cast<size_t>(strlen(message));
	token.line = scanner.line;

	return token;
}

static void skipWhitespace() {
	for (;;)
	{
		switch (char c = peek())
		{
			case ' ':
			case '\r':
			case '\t':
				advance();
				break;

			case '\n':
				++scanner.line;
				advance();
				break;

			case '/':
				if (peekNext() == '/')
				{
					// A comment goes until the end of the line.
					while (peek() != '\n' && !atEnd()) advance();
				}
				else return;

				break;

			default:
				return;
		}
	}
}

static TokenType checkKeyword(const size_t start, const size_t length, const char* rest, const TokenType type)
{
	if (scanner.current - scanner.start == start + length &&
		memcmp(scanner.start + start, rest, length) == 0)
		return type;

	return TOKEN_IDENTIFIER;
}

static TokenType identifierType()
{
	switch (scanner.start[0])
	{
		case 'a':
			return checkKeyword(1, 2, "nd", TOKEN_AND);
		case 'c':
			return checkKeyword(1, 4, "lass", TOKEN_CLASS);
		case 'd':
			return checkKeyword(1, 2, "ec", TOKEN_DEC);
		case 'e':
			return checkKeyword(1, 3, "lse", TOKEN_ELSE);
		case 'f':
			if (scanner.current - scanner.start > 1)
			{
				switch (scanner.start[1])
				{
					case 'a':
						return checkKeyword(2, 3, "lse", TOKEN_FALSE);
					case 'o':
						return checkKeyword(2, 1, "r", TOKEN_FOR);
					case 'u':
						return checkKeyword(2, 2, "nc", TOKEN_FUNC);
				}
			}
			break;
		case 'i':
			if (scanner.current - scanner.start > 1)
			{
				switch (scanner.start[1])
				{
					case 'f':
						return checkKeyword(2, 0, "", TOKEN_IF);
					case 'n':
						if (scanner.current - scanner.start > 2)
						{
							switch (scanner.start[2])
							{
								case 'c':
									return checkKeyword(3, 0, "", TOKEN_INC);
								case 'f':
									return checkKeyword(3, 0, "", TOKEN_INF);
							}
						}
						break;
				}
			}
			break;
		case 'o':
			return checkKeyword(1, 1, "r", TOKEN_OR);
		case 'p':
			return checkKeyword(1, 4, "rint", TOKEN_PRINT);
		case 'r':
			return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
		case 's':
			return checkKeyword(1, 4, "uper", TOKEN_SUPER);
		case 't':
			if (scanner.current - scanner.start > 1)
			{
				switch (scanner.start[1])
				{
					case 'h':
						return checkKeyword(2, 2, "is", TOKEN_THIS);
					case 'r':
						return checkKeyword(2, 2, "ue", TOKEN_TRUE);
				}
			}
			break;
		case 'v':
			return checkKeyword(1, 2, "ar", TOKEN_VAR);
		case 'w':
			return checkKeyword(1, 4, "hile", TOKEN_WHILE);
		case 'z':
			return checkKeyword(1, 4, "ilch", TOKEN_ZILCH);
	}
	return TOKEN_IDENTIFIER;
}

static Token identifier()
{
	while (isAlpha(peek()) || isDigit(peek()))
		advance();
	return makeToken(identifierType());
}


static Token number()
{
	while (isDigit(peek()))
		advance();

	// Look for a fractional part.
	if (peek() == '.' && isDigit(peekNext()))
	{
		// Consume the period.
		advance();

		while (isDigit(peek()))
			advance();
	}

	return makeToken(TOKEN_NUMBER);
}

static Token string()
{
	while (peek() != '"' && !atEnd())
	{
		if (peek() == '\n') ++scanner.line;
		advance();
	}

	if (atEnd()) return errorToken("Unterminated string.");

	// The closing quote.
	advance();
	return makeToken(TOKEN_STRING);
}

Token scanToken()
{
	skipWhitespace();
	scanner.start = scanner.current;

	if (atEnd())
		return makeToken(TOKEN_EOF);

	const char c = advance();
	if (isAlpha(c)) return identifier();
	if (isDigit(c)) return number();

	switch (c)
	{
		case '(': return makeToken(TOKEN_LEFT_PAREN);
		case ')': return makeToken(TOKEN_RIGHT_PAREN);

		case '{': return makeToken(TOKEN_LEFT_BRACE);
		case '}': return makeToken(TOKEN_RIGHT_BRACE);

		case ',': return makeToken(TOKEN_COMMA);
		case '.': return makeToken(TOKEN_DOT);

		case ';': return makeToken(TOKEN_SEMICOLON);

		case '+': return makeToken(TOKEN_PLUS);
		case '-': return makeToken(TOKEN_MINUS);
		case '/': return makeToken(TOKEN_SLASH);
		case '*': return makeToken(TOKEN_STAR);

		case '!':
			return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
		case '=':
			return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
		case '<':
			return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
		case '>':
			return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

		case '"': return string();

		default: return errorToken("Unexpected character.");;
	}
}