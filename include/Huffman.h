#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "Image.h"
#include "Node.h"

#include <vector>
#include <fstream>

class Huffman
{
private:
	std::vector<std::pair<uint32_t, std::vector<bool>>> codeVec;
	std::vector<std::pair<uint32_t, uint32_t>> colorFreqs;

	// Empty huffman data;
	void clear();

	// Huffman algorithm's methods
	void countFreq(const Image &);
	void generateCodes(const Node *node, std::vector<bool> &code);
	void buildTree();

	// Debug
	void printCodes() const;

	// Store/load informations needed from file to use Huffman compression
	void saveHuffHeader(std::ofstream &ofile) const;
	void readHuffHeader(std::ifstream &ifile);

	// Save/load data from/to file
	void saveCodes(std::ofstream &ofile, const Image &) const;
	void readCodes(std::ifstream &ifile, Image &);

public:
	Huffman();

	// Public interface
	void encode(std::ofstream &, const Image &);
	void decode(std::ifstream &, Image &);
};


#endif // !HUFFMAN_H

