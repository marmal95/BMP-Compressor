#include "Image.h"
#include "CText.h"
#include "RuntimeError.h"

#include <sstream>  // thrown errors' messages

void Image::swap(Image &img)
{
#ifdef _DEBUG
	std::cout << " -> [Image::swap]" << std::endl;
#endif // _DEBUG
	
	std::swap(surface, img.surface);
}

SDL_Surface * Image::create(unsigned int width, unsigned int height, unsigned int depth) const
{
#ifdef _DEBUG
	std::cout << " -> [Image::create]: Creating new SDL_Surface." << std::endl;
#endif
	SDL_Surface *img = SDL_CreateRGBSurface(0, static_cast<int>(width), static_cast<int>(height), static_cast<int>(depth), 0, 0, 0, 0);
	if (img == nullptr)
		throw RuntimeError();
	return img;
}

SDL_Surface * Image::copy(const SDL_Surface *img) const
{

#ifdef _DEBUG
	std::cout << " -> [Image::copy]: Copying SDL_Surface to new SDL_Surface." << std::endl;
#endif

	if (img == nullptr)
	{
#ifdef _DEBUG
		std::cerr  << "!!! [Image::copy]: " << CText("Copying not existing surface.") << std::endl;
#endif
		return nullptr;
	}

	SDL_Surface* new_img;
	int w = img->w, h = img->h;

	// Allocate empty surface
	new_img = create(w, h, img->format->BitsPerPixel);
	
	// Fast copy raw pixel data
	SDL_memcpy(new_img->pixels, img->pixels, (img->h * img->pitch));

	return new_img;
}

Image::Image()
	: surface(nullptr)
{
#ifdef _DEBUG
	std::cout << "[Image]: Called default constructor." << std::endl;
#endif // DEBUG
}

Image::Image(unsigned int width, unsigned int height, unsigned int depth)
	:surface(create(width, height, depth))
{
#ifdef _DEBUG
	std::cout << "[Image]: Called create empty surface constructor." << std::endl;
#endif // DEBUG
}

Image::Image(SDL_Surface *moved_surface)
	: Image()
{
#ifdef _DEBUG
	std::cout << "[Image]: Called SDL_Surface* move [" << CText("hardcoded swap", CText::Color::YELLOW) << "] constructor." << std::endl;
#endif // DEBUG

	// Zero-out given pointer, and attach surface
	std::swap(surface, moved_surface);
}

Image::Image(const SDL_Surface *img)
	: surface(copy(img))
{
#ifdef _DEBUG
	std::cout << "[Image]: Called SDL_Surface* copy constructor." << std::endl;
#endif // DEBUG
}

Image::Image(const Image &img)
	: surface(copy(img.surface))
{
#ifdef _DEBUG
	std::cout << "[Image]: Called copy constructor." << std::endl;
#endif // _DEBUG
}

Image::Image(Image &&img)
	: Image()
{
#ifdef _DEBUG
	std::cout << "[Image]: Called better move constructor." << std::endl;
#endif // _DEBUG

	// Zero-out moved image, and attach its data
	swap(img);
}

Image & Image::operator=(Image img)
{
#ifdef _DEBUG
	std::cout << " -> [Image::operator=]: Called universal assigment operator." << std::endl;
#endif // _DEBUG
	
	swap(img);
	return *this;
}

Image::~Image()
{

#ifdef _DEBUG
	std::cout << " -> [" << CText("~Image", CText::Color::MAGENTA) << "]" << ((surface != nullptr) ? ": Deallocated SDL_Surface." : "")  << std::endl;
#endif // _DEBUG

	// Remarks: it is safe to pass NULL to SDL_FreeSurface function
	// @see https://wiki.libsdl.org/SDL_FreeSurface#Remarks
	SDL_FreeSurface(surface);
	surface = nullptr;
}

void Image::printDetails(std::ostream &out) const
{
#ifdef _DEBUG
	out << " -> [Image::printDetails]" << std::endl;
#endif
	out << " - Empty: " << (empty() ? "true" : "false") << std::endl
		<< " - Width: " << width() << std::endl
		<< " - Height: " << height() << std::endl
		<< " - Depth: " << depth() << std::endl
		<< " - BPP: " << bpp() << std::endl
		<< " - Size: " << size() << std::endl;
}

const SDL_Surface * Image::img() const
{
	return surface;
}

SDL_Texture * Image::texture(SDL_Renderer *renderer) const
{
#ifndef _DEBUG
	return SDL_CreateTextureFromSurface(renderer, surface);
#else
	std::cout << " -> [Image::texture]: Creating texture." << std::endl;
	SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, surface);
	if (text == nullptr)
		std::cerr << "!!! [Image::texture]: " << CText("Failed to create texture.") << std::endl;
	return text;
#endif
}

unsigned int Image::width() const
{
	return empty() ? 0 : static_cast<unsigned int>(surface->w);
}

unsigned int Image::height() const
{
	return empty() ? 0 : static_cast<unsigned int>(surface->h);
}

unsigned int Image::bpp() const
{
	return empty() ? 0 : static_cast<unsigned int>(surface->format->BytesPerPixel);
}

unsigned int Image::depth() const
{
	return empty() ? 0 : static_cast<unsigned int>(surface->format->BitsPerPixel);
}

size_t Image::size() const
{
	return empty() ? 0 : (width() * height() * bpp());
}

bool Image::empty() const
{
	return surface == nullptr;
}

Image::pixel_iterator::pixel_iterator(SDL_Surface * surface)
	: s(surface), x(0), y(0),
	// Gets first pixel data pointer
	current(reinterpret_cast<uint8_t *>(s->pixels))
{}

Image::pixel_iterator::pixel_iterator(SDL_Surface * surface, size_t x, size_t y)
	: s(surface), x(x), y(y),
	// Gets current pixel data pointer from SDL_Surface->pixels
	current(reinterpret_cast<uint8_t *>(s->pixels) + y * s->pitch + x * s->format->BytesPerPixel)
{}

Image::pixel_iterator & Image::pixel_iterator::operator++()
{
	++x;
	if (x != static_cast<size_t>(s->w))
	{
		current += s->format->BytesPerPixel;
	}
	else
	{
		x = 0;
		++y;
		current = reinterpret_cast<uint8_t *>(s->pixels) + y * s->pitch;
	}

	return *this;
}

Image::pixel_iterator Image::pixel_iterator::operator++(int)
{
	pixel_iterator ret = *this;

	++x;
	if (x != static_cast<size_t>(s->w))
	{
		current += s->format->BytesPerPixel;
	}
	else
	{
		x = 0;
		++y;
		current = reinterpret_cast<uint8_t *>(s->pixels) + y * s->pitch;
	}

	return std::move(ret);
}

inline std::pair<size_t, size_t> Image::pixel_iterator::xy() const
{
	return std::make_pair(x, y);
}

std::array<uint8_t, 3> Image::pixel_iterator::rgb2() const
{
	uint32_t val = value2();

	// Gets component's data using SDL_Surface schema
	return std::array<uint8_t, 3>{
		static_cast<uint8_t>(((val & s->format->Rmask) >> s->format->Rshift) << s->format->Rloss),
		static_cast<uint8_t>(((val & s->format->Gmask) >> s->format->Gshift) << s->format->Gloss),
		static_cast<uint8_t>(((val & s->format->Bmask) >> s->format->Bshift) << s->format->Bloss)
	};
}

uint32_t Image::pixel_iterator::value() const
{

#ifdef _DEBUG
	if (x >= static_cast<size_t>(s->w) || y >= static_cast<size_t>(s->h))
	{
		std::cerr << "!!! [Image::pixel_iterator::value]: " << CText("Wanted pixel out of Image range.") << std::endl;
		return 0;
	}
#endif

	// Gets pixel's data depending on bpp (returning 8|16|24|32 significant bits)
	switch (s->format->BytesPerPixel)
	{
	case 1:
		return *current;
	case 2:
		return *reinterpret_cast<uint16_t *>(current);
	case 3:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		return current[0] << 16 | current[1] << 8 | current[2];
#else
		return current[0] | current[1] << 8 | current[2] << 16;
#endif
	case 4:
		return *reinterpret_cast<uint32_t *>(current);
	default:
		return 0;
	}
}

uint32_t Image::pixel_iterator::value2() const
{

#ifdef _DEBUG
	if (x >= static_cast<size_t>(s->w) || y >= static_cast<size_t>(s->h))
	{
		std::cerr << "!!! [Image::pixel_iterator::value]: " << CText("Wanted pixel out of Image range.") << std::endl;
		return 0;
	}

	if (s->format->BytesPerPixel != 2)
	{
		std::ostringstream os;
		os << "Trying to get 16 bit pixel's value of " << s->format->BitsPerPixel << " bit depth image.";
		throw RuntimeError(os.str());
	}
#endif

	// Gets pixel's data of 2 bpp image
	return *reinterpret_cast<uint16_t *>(current);
}

uint8_t Image::pixel_iterator::gray2() const
{
	SDL_Color col = color2();
	return static_cast<uint8_t>(0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b);
}

SDL_Color Image::pixel_iterator::color() const
{
	uint32_t RGB = value();

	// Gets component's data using SDL_Surface schema
	SDL_Color COLOR = { 0, 0, 0, 0 };
	if (s->format->palette == nullptr)
	{
		COLOR.r = ((RGB & s->format->Rmask) >> s->format->Rshift) << s->format->Rloss;
		COLOR.g = ((RGB & s->format->Gmask) >> s->format->Gshift) << s->format->Gloss;
		COLOR.b = ((RGB & s->format->Bmask) >> s->format->Bshift) << s->format->Bloss;
	}
	// for palletized surfaces (depth <= 8 bit)
	else if (RGB < static_cast<uint32_t>(s->format->palette->ncolors))
		return s->format->palette->colors[RGB];

	return COLOR;
}

SDL_Color Image::pixel_iterator::color2() const
{
	uint32_t RGB = value2();

	return SDL_Color{ 
		static_cast<uint8_t>(((RGB & s->format->Rmask) >> s->format->Rshift) << s->format->Rloss),
		static_cast<uint8_t>(((RGB & s->format->Gmask) >> s->format->Gshift) << s->format->Gloss),
		static_cast<uint8_t>(((RGB & s->format->Bmask) >> s->format->Bshift) << s->format->Bloss)
	};
}

void Image::pixel_iterator::value(uint32_t RGB)
{

#ifdef _DEBUG
	if (x >= static_cast<size_t>(s->w) || y >= static_cast<size_t>(s->h))
	{
		std::cerr << "!!! [Image::pixel_iterator::value]: " << CText("Setting pixel out of Image range.") << std::endl;
		return;
	}
#endif

	// Sets pixel data depending on bpp
	switch (s->format->BytesPerPixel)
	{
	case 1:
		*current = RGB;
		return;

	case 2:
		*reinterpret_cast<uint16_t*>(current) = RGB;
		return;

	case 3:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		current[0] = (RGB >> 16) & 0xff;
		current[1] = (RGB >> 8) & 0xff;
		current[2] = RGB & 0xff;
#else
		current[0] = RGB & 0xff;
		current[1] = (RGB >> 8) & 0xff;
		current[2] = (RGB >> 16) & 0xff;
#endif
		return;

	case 4:
		*reinterpret_cast<uint32_t*>(current) = RGB;
		return;
	}
}

void Image::pixel_iterator::value2(uint32_t RGB2) const
{
#ifdef _DEBUG
	if (x >= static_cast<size_t>(s->w) || y >= static_cast<size_t>(s->h))
	{
		std::cerr << "!!! [Image::pixel_iterator::value]: " << CText("Setting pixel out of Image range.") << std::endl;
		return;
	}

	if (s->format->BytesPerPixel != 2)
	{
		std::ostringstream os;
		os << "Trying to set 16 bit pixel's value to " << s->format->BitsPerPixel << " bit depth image.";
		throw RuntimeError(os.str());
	}
#endif

	// Sets pixel's data of 12 bit (depth) image
	*reinterpret_cast<uint16_t*>(current) = RGB2;
}

void Image::pixel_iterator::value2(uint8_t R4, uint8_t G4, uint8_t B4)
{
	// Convert to full width 32 bit pixel
	uint32_t RGB = (R4 >> s->format->Rloss) << s->format->Rshift
		| (G4 >> s->format->Gloss) << s->format->Gshift
		| (B4 >> s->format->Bloss) << s->format->Bshift
		| s->format->Amask;
	
	value2(RGB);
}

bool Image::pixel_iterator::operator==(const pixel_iterator & it) const
{
	return s == it.s && x == it.x && y == it.y;
}

bool Image::pixel_iterator::operator!=(const pixel_iterator & it) const
{
	return x != it.x || y != it.y || s != it.s;
}

bool Image::pixel_iterator::operator<(const pixel_iterator & it) const
{
	return s == it.s && ((y < it.y) || (y == it.y && x < it.x));
}

Image::pixel_iterator Image::begin() const
{
	return pixel_iterator(surface);
}

Image::pixel_iterator Image::end() const
{
	return pixel_iterator(surface, 0, surface->h);
}
