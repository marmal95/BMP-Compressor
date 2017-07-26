#include "CText.h"

CText::CText()
	:text(), color(Color::RESET)
{
}

CText::CText(const char * txt, Color col)
	:text(txt), color(col)
{}

CText::CText(const std::string & str, Color col)
	:text(str), color(col)
{
}

std::ostream & CText::print(std::ostream & os) const
{
	bool sterr = (&os == &std::cerr);
#ifdef __unix
	os << colors[static_cast<size_t>((color == Color::RESET && sterr) ? Color::RED : color)] << text << colors[static_cast<size_t>(Color::RESET)];
#elif _WIN32
	HANDLE stdhand = GetStdHandle(sterr ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(stdhand, colors[static_cast<size_t>((color == Color::RESET && sterr) ? Color::RED : color)]);
	os << text;
	SetConsoleTextAttribute(stdhand, colors[static_cast<size_t>(Color::RESET)]);

#endif
	return os;
}

std::ostream & operator<<(std::ostream & os, const CText & ctext)
{
	return ctext.print(os);
}
