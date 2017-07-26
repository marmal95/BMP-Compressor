#include "BMP.h"

#include <utility>

void BMP::store(const std::string & filename, const Image & image) const
{
#ifdef _DEBUG
	std::cout << " -> [BMP::store]: SDL_SaveBMP()" << std::endl;
#endif
	
	// Remarks: I know it is undefined behaviour but SDL_SaveBMP() requires [not const] SDL_Surface*
	// even though it only access it in read mode (const-incorrect API)
	SDL_SaveBMP(const_cast<SDL_Surface*>(image.img()), filename.c_str());
}

Image BMP::recover(const std::string & filename)
{
#ifdef _DEBUG
	std::cout << " -> [BMP::recover]: SDL_LoadBMP()" << std::endl;
#endif

	return Image(SDL_LoadBMP(filename.c_str()));
}

std::string BMP::extension() const
{
	return std::string(".bmp");
}

BMP::BMP(const ImageHandler &img)
	: ImageHandler(img)
{
#ifdef _DEBUG
	std::cout << "[BMP]: Called copy constructor." << std::endl;
#endif
}

BMP::BMP(ImageHandler &&img)
	: ImageHandler(std::move(img))
{
#ifdef _DEBUG
	std::cout << "[BMP]: Called move constructor." << std::endl;
#endif
}

BMP & BMP::operator=(const ImageHandler &img)
{
#ifdef _DEBUG
	std::cout << " -> [BMP::operator=]: Called copy assigment operator." << std::endl;
#endif

	ImageHandler::operator=(img);
	return *this;
}

BMP & BMP::operator=(ImageHandler &&img)
{
#ifdef _DEBUG
	std::cout << " -> [BMP::operator=]: Called move assigment operator." << std::endl;
#endif

	ImageHandler::operator=(std::move(img));
	return *this;
}