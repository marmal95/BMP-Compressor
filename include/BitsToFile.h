#ifndef BITS_TO_FILE_H
#define BITS_TO_FILE_H

#include <fstream>
#include <vector>

class BitsToFile
{
private:
	char c;
	int pos;
	std::ofstream &file;
	BitsToFile &write();

public:
	BitsToFile(std::ofstream &f);
	BitsToFile &to(bool f);
	BitsToFile &flush();
	void to(const std::vector<bool> &vec);
};

class BitsFromFile
{
private:
	std::vector<char> buffer;
	std::vector<char>::iterator c;
	short pos;
	std::vector<char> read(std::ifstream &f) const;

public:
	BitsFromFile(std::ifstream &f);
	bool get();
};

#endif