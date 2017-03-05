#include "Huffman.h"
#include "BitsToFile.h"

#include <iostream>
#include <iomanip> // printCodes
#include <queue>
#include <algorithm> // sort

Huffman::Huffman()
	:	codeVec(std::vector<std::pair<uint32_t, std::vector<bool>>>()),
		colorFreqs(std::vector<std::pair<uint32_t, uint32_t>>())
{}

void Huffman::encode(std::ofstream &ofile, const Image &image)
{
#ifdef _DEBUG
	std::cout << "\n=== HUFFMAN COMPRESSION ===" << std::endl;
#endif

	// Huffman algorithm
	countFreq(image); // colorFreqs
	buildTree(); //codeVec

	// Save compressed data to file
	saveHuffHeader(ofile);
	saveCodes(ofile, image);

	// Clear generated data
	clear();

#ifdef _DEBUG
	std::cout << "=== HUFFMAN COMPRESSION DONE ===\n" << std::endl;
#endif
}

void Huffman::decode(std::ifstream &ifile, Image &image)
{
#ifdef _DEBUG
	std::cout << "\n=== HUFFMAN DECOMPRESSION ===" << std::endl;
#endif

	// Generate data
	readHuffHeader(ifile); // read colorFreqs
	buildTree(); //create codeVec

	readCodes(ifile, image); // read data

	// Clear generated data
	clear();

#ifdef _DEBUG
	std::cout << "=== HUFFMAN DECOMPRESSION DONE ===\n" << std::endl;
#endif
}

void Huffman::clear()
{
	codeVec.clear();
	colorFreqs.clear();
}

void Huffman::countFreq(const Image& image)
{
#ifdef _DEBUG
	std::cout << "Counting colors..." << std::endl;
#endif

	bool found;
	uint32_t color;
	auto img_end = image.end();

	for (auto pixel_it = image.begin(); pixel_it < img_end; ++pixel_it)
	{
		found = false;
		color = pixel_it.value2();

		for (auto &v : colorFreqs) // check if already appeared
		{
			if (v.first == color) // if found - increment frequency
			{
				++v.second;
				found = true;
				break;
			}
		}

		if (!found) // if not found - Add it
			colorFreqs.push_back(std::make_pair(color, 1));
	}

#ifdef _DEBUG
	std::cout << "Number of colors: ." << colorFreqs.size() << std::endl;
#endif
}

void Huffman::generateCodes(const Node *node, std::vector<bool>& code)
{
	if (node == nullptr)
		return;
	if (node->right == nullptr && node->left == nullptr) // is leaf - save code of node
		codeVec.push_back(std::make_pair(node->colorData.first, code));
	else
	{
		auto leftPref = code;
		leftPref.push_back(false);
		generateCodes(node->left, leftPref); // add 0 and go left

		auto rightPref = code;
		rightPref.push_back(true);
		generateCodes(node->right, rightPref); // add 1 and go right
	}
}

void Huffman::buildTree()
{
#ifdef _DEBUG
	std::cout << "Building tree..." << std::endl;
#endif

	// Add all colors as single nodes
	std::priority_queue<Node*, std::vector<Node*>, NodeCmp> trees; 
	for (auto &v : colorFreqs)
		trees.push(new Node(v));

	if (trees.size() == 1)
	{
		auto chR = trees.top();
		trees.pop();

		auto temp = new Node(std::make_pair(0, 0));

		auto chP = new Node(chR, temp);
		trees.push(chP);

	}

	// Build Main Tree
	while (trees.size() > 1)
	{
		auto chR = trees.top();
		trees.pop();

		auto chL = trees.top();
		trees.pop();

		auto chP = new Node(chR, chL);
		trees.push(chP);
	}

	auto root = trees.top();

#ifdef _DEBUG
	std::cout << "Tree build." << std::endl;
	std::cout << "Generating codes & sorting..." << std::endl;
#endif

	std::vector<bool> codes; // code for each color
	generateCodes(root, codes);

	delete root; // no longer needed

	// sort codes before writing to file
	// speed up decoding - most frequent codes are in the beginning
	std::sort(codeVec.begin(), codeVec.end(),
		[](const std::pair<uint32_t, std::vector<bool>> &p1, const std::pair<uint32_t, std::vector<bool>> &p2) -> bool
	{
		size_t p1_size = p1.second.size(),
			p2_size = p2.second.size();

		if (p1_size < p2_size)
			return true;
		if (p1_size > p2_size)
			return false;

		for (size_t i = 0; i < p1_size; ++i)
		{
			if (p1.second[i] < p2.second[i])
				return true;
			else if (p1.second[i] > p2.second[i])
				return false;
		}
		return true;
	}
	);

#ifdef _DEBUG
	printCodes();
	std::cout << "Codes generated." << std::endl;
#endif

}

void Huffman::printCodes() const
{
	auto prev = std::cout.fill();
	std::cout << "Huffman encoding map:" << std::endl << std::endl;
	for (const auto &v : codeVec)
	{
		std::cout << std::hex << std::setfill('0') << std::setw(6) << v.first << "   ";
		for (const auto &vv : v.second)
			std::cout << vv;
		std::cout << std::dec << std::endl;
	}

	// back to previous fill
	std::cout.fill(prev);
}

void Huffman::saveHuffHeader(std::ofstream &ofile) const
{
#ifdef _DEBUG
	std::cout << "Saving huffman header..." << std::endl;
#endif

	uint32_t clr;
	unsigned int cntr;
	size_t numberOfColors = colorFreqs.size();

	ofile.write(reinterpret_cast<const char*>(&numberOfColors), sizeof(numberOfColors));

	for (auto &v : colorFreqs)
	{
		clr = v.first;
		cntr = v.second;
		ofile.write(reinterpret_cast<const char*>(&clr), sizeof(clr));
		ofile.write(reinterpret_cast<const char*>(&cntr), sizeof(cntr));
	}

#ifdef _DEBUG
	std::cout << "Huffman header saved." << std::endl;
#endif
}

void Huffman::readHuffHeader(std::ifstream &ifile)
{
#ifdef _DEBUG
	std::cout << "Reading huffman header..." << std::endl;
#endif

	uint32_t clr, cntr;
	size_t numOfColors;

	ifile.read((char*)(&numOfColors), sizeof(numOfColors));

	for (size_t i = 0; i < numOfColors; ++i)
	{
		ifile.read(reinterpret_cast<char *>(&clr), sizeof(clr));
		ifile.read(reinterpret_cast<char *>(&cntr), sizeof(cntr));
		colorFreqs.push_back(std::make_pair(clr, cntr));
	}

#ifdef _DEBUG
	std::cout << "Huffman header read." << std::endl;
#endif
}

void Huffman::saveCodes(std::ofstream &ofile, const Image &image) const
{
#ifdef _DEBUG
	std::cout << "Saving content..." << std::endl;
#endif

	uint32_t color;
	BitsToFile btf(ofile);

	auto img_end = image.end();
	for (auto pixel_it = image.begin(); pixel_it < image.end(); ++pixel_it)
	{
		color = pixel_it.value2();
		for (auto &v : codeVec)
		{
			if (v.first == color)
			{
				btf.to(v.second);
				break;
			}
		}

	}

	btf.flush();
#ifdef _DEBUG
	std::cout << "Content saved." << std::endl;
#endif
}

void Huffman::readCodes(std::ifstream &ifile, Image &image)
{
#ifdef _DEBUG
	std::cout << "Reading content..." << std::endl;
#endif

	BitsFromFile bff(ifile);
	std::vector<bool> vec;
	bool found;
	auto img_end = image.end();

	for (auto pixel_it = image.begin(); pixel_it < img_end; ++pixel_it)
	{
		found = false;
		while (!found)
		{
			vec.push_back(bff.get());
			for (auto &v : codeVec)
			{
				if (v.second == vec)
				{
					pixel_it.value2(v.first);
					found = true;
					break;
				}
			}
		}
		vec.clear();
	}

#ifdef _DEBUG
	std::cout << "Content read." << std::endl;
#endif
}

