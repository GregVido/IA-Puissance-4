#pragma once
#include <iostream>
#define NOMINMAX
#include <Windows.h>
#include "resource.h"
#include <conio.h>
#include <array>

inline int getConsoleWidth()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi{};

	if (GetConsoleScreenBufferInfo(
		GetStdHandle(STD_OUTPUT_HANDLE),
		&csbi))
	{
		return csbi.srWindow.Right - csbi.srWindow.Left + 1;
	}

	return 80;
}

inline int getUtf8VisualLength(const std::string& text)
{
	int length = 0;

	for (unsigned char c : text)
	{
		// Un caractère UTF-8 commence lorsque les deux bits de poids fort
		// ne sont pas "10".
		if ((c & 0xC0) != 0x80)
			++length;
	}

	return length;
}

inline void printCentered(const std::string& text)
{
	const int consoleWidth = getConsoleWidth();

	const int textWidth =
		getUtf8VisualLength(text);

	const int padding =
		std::max(
			0,
			(consoleWidth - textWidth) / 2
		);

	std::cout
		<< std::string(padding, ' ')
		<< text
		<< '\n';
}

inline void printCenteredColored(
	const std::string& text,
	const char* style = "",
	const char* reset = "\033[0m")
{
	const int consoleWidth = getConsoleWidth();
	const int textWidth = getUtf8VisualLength(text);

	const int padding =
		std::max(0, (consoleWidth - textWidth) / 2);

	std::cout
		<< std::string(padding, ' ')
		<< style
		<< text
		<< reset
		<< '\n';
}