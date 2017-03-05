#include "RGB12.h"
#include "LZ77.h"
#include "Huffman.h"
#include "CText.h"
#include "RuntimeError.h"

#include <iostream>
#include <utility>
#include <sstream>

//const unsigned int RGB12::supported_depth = 12;

Image RGB12::convert(const Image& img) const
{
	// More usefull when don't throw RuntimError
	if (img.empty())
	{
#ifdef _DEBUG
		std::cerr << "!!! [RGB12::convert]: " << CText("Cannot convert not initialized Image.") << std::endl;
#endif
		return img;
	}

	// Simply copy surface if it is already in proper format
	if (img.depth() == RGB12::supported_depth)
		return img;

#ifdef _DEBUG
	std::cout << " -> [RGB12::convert]: Converting Image to RGB444 format." << std::endl;
#endif

	// Start conversion
	Image converted(img.width(), img.height(), RGB12::supported_depth);
	SDL_Color color;
	auto conv_end = converted.end();
	auto pixel_const = img.begin();

	for (auto pixel = converted.begin(); pixel != conv_end; ++pixel, ++pixel_const)
	{
		color = pixel_const.color();
		color.r = (color.r >> 4) << 4;
		color.g = (color.g >> 4) << 4;
		color.b = (color.b >> 4) << 4;
		pixel.value2(color.r, color.g, color.b);
	}

	return std::move(converted);
}

RGB12 & RGB12::toGrayScale()
{
#ifdef _DEBUG
	std::cout << " -> [RGB12::toGrayScale]: Converting Image to grey scale." << std::endl;
#endif // _DEBUG

	uint8_t gray;
	auto img_end = image.end();
	for (auto pixel = image.begin(); pixel < img_end; ++pixel)
	{
		gray = (pixel.gray2() >> 4) << 4;
		pixel.value2(gray, gray, gray);
	}

	if (algorithm == Algorithm::BitDensity)
		algorithm = Algorithm::GrayScale;

	return *this;
}

void RGB12::load444(std::ifstream &f, Image &img)
{
#ifdef _DEBUG
	std::cout << " -> [RGB12::load444]: Run BitDensity load algorithm." << std::endl;
#endif

	std::vector<char> buffer = std::vector<char>(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());

	int colorToCode = 0;
	std::array<uint8_t, 3> pixel;
	auto pixel_iter = img.begin();

	for (auto i : buffer)
	{
		for (int k = 0; k < 2; ++k)
		{
			pixel[colorToCode] = k ? (i << 4) : ((i >> 4) << 4);
			++colorToCode;

			if (colorToCode == 3)
			{
				pixel_iter.value2(pixel[0], pixel[1], pixel[2]);
				++pixel_iter;
				colorToCode = 0;
			}
		}
	}
}

void RGB12::saveGray(std::ofstream & output, const Image & img) const
{
	auto img_end = img.end();
	uint8_t block;

	for (auto pixel = img.begin(); pixel < img_end; ++pixel)
	{
		block = (pixel.gray2() >> 4) << 4;
		++pixel;
		block |= pixel.gray2() >> 4;
		output.write(reinterpret_cast<char*>(&block), sizeof(block));
	}
}

void RGB12::loadGray(std::ifstream & input, Image & img)
{
	std::vector<char> buffer = std::vector<char>(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());

	auto it = buffer.begin();
	auto img_end = img.end();

	uint8_t gray;
	for (auto pixel = img.begin(); pixel < img_end; ++pixel, ++it)
	{
		gray = (*it >> 4) << 4;
		pixel.value2(gray, gray, gray);
		++pixel;

		gray = *it << 4;
		pixel.value2(gray, gray, gray);
	}
		
}

void RGB12::save444(std::ofstream &f, const Image &img) const
{
#ifdef _DEBUG
	std::cout << " -> [RGB12::save444]: Run BitDensity save algorithm." << std::endl;
#endif

	bool isSpace = true,
		firstHalf = true;
	char usedChar = 0;
	std::array<uint8_t, 3> color;

	auto img_end = img.end();
	for (auto pixel = img.begin(); pixel < img_end; ++pixel)
	{
		color = pixel.rgb2();
		for (int k = 0; k < 3; ++k)
		{
			if (firstHalf)
			{
				usedChar = (color[k] >> 4) << 4;
				firstHalf = false;
			}
			else
			{
				usedChar |= color[k] >> 4;
				firstHalf = true;
				isSpace = false;
			}

			if (!isSpace)
			{
				// Binary save
				f.write(&usedChar, sizeof(usedChar));
				isSpace = true;
				usedChar = 0;
			}
		}
	}

	if (isSpace)
		f.write(&usedChar, sizeof(usedChar));
}

void RGB12::store(const std::string & filename, const Image & img) const
{
#ifdef _DEBUG
	std::cout << "\n -> [RG12::store]: Storing Image to file process has just begun." << std::endl;
#endif
	std::ofstream f;

	// Load file to save data in binary mode
	openStream(filename, f);

	// Save global header needed to recover Image
	writeHeader(f, img, algorithm);

	// Save by chosen (or default) algorithm
	switch (algorithm)
	{
	case Algorithm::BitDensity:
		save444(f, img);
		break;
	case Algorithm::Huffman:
	{
		Huffman huffman;
		huffman.encode(f, img);
		break;
	}
	case Algorithm::LZ77:
	{
		LZ77 lz77;
		lz77.encode(f, img);
		break;
	}
	case Algorithm::GrayScale:
		saveGray(f, img);
		break;
	}

	// Close file
	f.close();
#ifdef _DEBUG
	std::cout << " <- [RGB12::store]: Finished.\n" << std::endl;
#endif
}

Image RGB12::recover(const std::string & filename)
{
#ifdef _DEBUG
	std::cout << "\n -> [RG12::recover]: Recovering Image from file process has just begun." << std::endl;
#endif
	std::ifstream f;

	// Open input file in binary mode
	openStream(filename, f);

	// Read global header data
	unsigned int width, height;
	Algorithm alg;
	std::tie(width, height, alg) = readHeader(f);

	// Create new empty Image
	Image recovered(width, height, RGB12::supported_depth);

	// Load depending on the alogrithm
	switch (alg)
	{
	case Algorithm::BitDensity:
		load444(f, recovered);
		break;
	case Algorithm::Huffman:
	{
		Huffman huffman;
		huffman.decode(f, recovered);
		break;
	}
	case Algorithm::LZ77:
	{
		LZ77 lz77;
		lz77.decode(f, recovered);
		break;
	}
	case Algorithm::GrayScale:
		loadGray(f, recovered);
		break;
	default:
		std::ostringstream os;
		os << "Saved with uknown algorithm: [unsigned int] " << static_cast<unsigned int>(alg);
		throw RuntimeError(os.str());
	}

	// Close file
	f.close();

#ifdef _DEBUG
	std::cout << " <- [RG12::recover]: Finished.\n" << std::endl;
#endif

	// Return recovered Image
	return std::move(recovered);
}

/**
 * @return {
 *		unsigned int width,
 *		unsigned int height,
 *		uint8_t depth (bits per pixel),
 *		Algorithm chosen compression algorithm }
*/
std::tuple<unsigned int, unsigned int, RGB12::Algorithm> RGB12::readHeader(std::ifstream &input) const
{
#ifdef _DEBUG
	std::cout << " -> [RGB12::readHeader]: Getting stored informations about this file." << std::endl;
#endif

	// Read size of "verifier" string
	size_t str_size;
	input.read(reinterpret_cast<char *>(&str_size), sizeof(str_size));
	if (str_size >= 1000)
		throw RuntimeError("Header of processed file is possibly invaild.");

	// Read cstring "verifier" and convert it to "string"
	char *cstr = new char[str_size + 1];
	input.read(cstr, str_size);
	cstr[str_size] = '\0';
	std::string ext(cstr);
	delete[] cstr;

#ifdef _DEBUG
	std::cout << "- string(" << str_size << "): " << ext << std::endl;
#endif

	// Verify header
	if(!std::equal(ext.begin(), ext.end(), extension().begin()))
		throw RuntimeError("Header of processed file is not vaild.");

	// Load actually required things
	unsigned int width, height;
	Algorithm alg;

	input.read(reinterpret_cast<char *>(&width), sizeof(width));
	input.read(reinterpret_cast<char *>(&height), sizeof(height));
	input.read(reinterpret_cast<char *>(&alg), sizeof(alg));

#ifdef _DEBUG
	std::cout << "- Algorithm: " << static_cast<unsigned int>(alg) << std::endl;
	std::cout << "- Width: " << width << std::endl;
	std::cout << "- Height: " << height << std::endl;
#endif

	return std::make_tuple(width, height, alg);
}

void RGB12::writeHeader(std::ofstream &output, const Image &img, Algorithm alg) const
{

#ifdef _DEBUG
	std::cout << " -> [RGB12::writeHeader]: Save informations about Image to file." << std::endl;
	img.printDetails(std::cout);
	std::cout << " - Algorithm: " << static_cast<unsigned int>(alg) << std::endl;
#endif

	// Saving Image informations
	std::string ext = extension();
	size_t ext_size = ext.size();
	const unsigned int 
		width = img.width(), 
		height = img.height();

	output.write(reinterpret_cast<const char *>(&ext_size), sizeof(ext_size));
	output.write(ext.c_str(), ext_size);
	output.write(reinterpret_cast<const char *>(&width), sizeof(width));
	output.write(reinterpret_cast<const char *>(&height), sizeof(height));
	output.write(reinterpret_cast<const char *>(&alg), sizeof(alg));
}

std::string RGB12::extension() const
{
	return std::string(".rgb12");
}

RGB12::RGB12(Algorithm alg)
	: algorithm(alg)
{
#ifdef _DEBUG
	std::cout << "[RGB12]: Called default constructor." << std::endl;
#endif
}

RGB12::RGB12(const ImageHandler &img, Algorithm alg)
	: ImageHandler(convert(img.image)), algorithm(alg) // affect when Image is protected
{
#ifdef _DEBUG
	std::cout << "[RGB12]: Called convert ImageHandler constructor." << std::endl;
#endif
}

RGB12::RGB12(const RGB12 &rgb)
	: ImageHandler(rgb), algorithm(rgb.algorithm)
{
#ifdef _DEBUG
	std::cout << "[RGB12]: Called copy constructor." << std::endl;
#endif
}

RGB12::RGB12(RGB12 &&rgb)
	: ImageHandler(std::move(rgb)), algorithm(rgb.algorithm)
{
#ifdef _DEBUG
	std::cout << "[RGB12]: Called move constructor." << std::endl;
#endif
}

RGB12 & RGB12::operator=(const RGB12 &rgb)
{
#ifdef _DEBUG
	std::cout << " -> [RGB12::operator=]: Called copy assigment operator." << std::endl;
#endif
	
	ImageHandler::operator=(rgb);
	algorithm = rgb.algorithm;
	return *this;
}

RGB12 & RGB12::operator=(RGB12 &&rgb)
{
#ifdef _DEBUG
	std::cout << " -> [RGB12::operator=]: Called move assigment operator." << std::endl;
#endif
	ImageHandler::operator=(std::move(rgb));
	algorithm = rgb.algorithm;
	return *this;
}