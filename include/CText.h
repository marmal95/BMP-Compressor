#ifndef CTEXT_H
#define CTEXT_H

#include <iostream>
#include <array>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

class CText
{
private:
#ifdef __unix
	std::array<std::string, 9> colors = { 
		{
			"\033[30m", // black letters
			//"\033[30m", // gray letters
			"\033[31;1m", // red bright letters
			"\033[32;1m", // green bright letters
			"\033[33;1m", // yellow bright letters
			"\033[34;1m", // blue bright letters
			"\033[35;1m", // magneta bright letters
			"\033[36;1m", // cyan bright letters
			"\033[37;1m", // white letters

			"\033[0m" // RESET
		} 
	};
#elif _WIN32
	static constexpr const std::array<int, 9> colors = {
		{
			0, // black letters
			FOREGROUND_RED | FOREGROUND_INTENSITY, // bright red letters
			FOREGROUND_GREEN | FOREGROUND_INTENSITY, // bright green letters
			FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, // bright yellow letters
			FOREGROUND_BLUE | FOREGROUND_INTENSITY, // bright blue letters
			FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY, // bright magneta letters
			FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, // bright cyan letters
			FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED, // white letters

			FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED // RESET
		} 
	};
#endif

public:
	enum class Color : size_t
	{
		BLACK,
		RED,
		GREEN,
		YELLOW,
		BLUE,
		MAGENTA,
		CYAN,
		WHITE,
		RESET
	};
	const std::string text;
	const Color color;
	CText();
	CText(const char *txt, Color col = Color::RESET);
	CText(const std::string &str, Color col = Color::RESET);
	std::ostream& print(std::ostream &os) const;

};

std::ostream& operator<<(std::ostream &os, const CText &ctext);

#endif // !C_TEXT_H