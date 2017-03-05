#ifndef IMAGE_H
#define IMAGE_H

#include "SDL_Local.h"
#include <iostream>
#include <array>

class Image
{
private:

	// Swapper for better class construction
	void swap(Image &);

protected:

	/**
	 * Pointer to SDL_Surface structure containing pixel data 
	 * and every needed informations to process an image
	 */
	SDL_Surface *surface;

	/// SDL Utility functions for private purposes 
	/// (they do NOT modify the object directly - so they are const)

	/**
	 * Creates an empty SDL_Surface (RGB) with paramters
	 * @param width of the surface
	 * @param height of the surface
	 * @param color depth of the surface (number of bits each pixel takes in memory)
	 * @return pointer to newly allocated SDL_Surface structure
	 * @throws RuntimError when allocation fails
	 */
	SDL_Surface *create(unsigned int width, unsigned int height, unsigned int depth) const;

	/**
	 * Creates a copy of existing SDL_Surface strucutre
	 * @return pointer to newly allocated SDL_Surface structure
	 * @throws RuntimError when allocation fails
	 */
	SDL_Surface *copy(const SDL_Surface *) const;

public:

	class pixel_iterator
	{
	private:
		SDL_Surface *s;
		size_t x, y;
		uint8_t *current;

	public:
		pixel_iterator(SDL_Surface *surface);
		pixel_iterator(SDL_Surface *surface, size_t x, size_t y);

		pixel_iterator(const pixel_iterator& ) = default;
		pixel_iterator(pixel_iterator &&) = default;
		pixel_iterator& operator=(const pixel_iterator &) = default;
		pixel_iterator& operator=(pixel_iterator &&) = default;
		~pixel_iterator() = default;

		pixel_iterator& operator++(); // pre increment
		pixel_iterator operator++(int); // post increment

		/**
		 * Gets current [x, y] coordinates on SDL_Surface
		 * @return pair<size_t, size_t> first => x, second => y coordinate
		 */
		std::pair<size_t, size_t> xy() const;

		/**
		 * Gets pixel data in one dimensional array format
		 * where keys: [0] => red component, [1] => green component, [2] => blue component
		 * @return array<uint8_t, 3>
		 *
		 * Remarks: Only for 2 bytes per pixel Image (otherwise undefined behaviour)
		 */
		std::array<uint8_t, 3> rgb2() const;

		/**
		 * Gets pixel's data (32 bits)
		 * @return uint32_t 
		 */
		uint32_t value() const;

		/**
		 * value() specialization
		 *
		 * Remarks: Only for 2 bytes per pixel Image (otherwise undefined behaviour)
		 */
		uint32_t value2() const;

		/**
		 * Gets gray scale color using stanard:
		 * @link https://en.wikipedia.org/wiki/Grayscale#Luma_coding_in_video_systems 
		 * 
		 * @return uint8_t grey scale color made from RGB components
		 *
		 * Remarks: Only for 2 bytes per pixel Image (otherwise undefined behaviour)
		 */
		uint8_t gray2() const;


		/**
		 * Gets pixel data in SDL_Color format
		 * @return SDL_Color
		 */
		SDL_Color color() const;
		
		/**
		 * color() specliaziation
		 *
		 * Remarks: Only for 2 bytes per pixel Image (otherwise undefined behaviour)
		 */
		SDL_Color color2() const;

		/**
		 * Sets pixel's data to SDL_Surface
		 * @param pixel data (32 bits)
		 */
		void value(uint32_t RGB);

		/**
		 * value(uint32_t) specialization
		 *
		 * Remarks: Only for 2 bytes per pixel Image (otherwise undefined behaviour)
		 */
		void value2(uint32_t RGB2) const;

		/**
		 * Sets pixel's color's data to SDL_Surface (Doesn't work with PALLETIZED SDL_Surface)
		 * @param red component color value (8 bit)
		 * @param green component color value (8 bit)
		 * @param blue component color value (8 bit)
		 *
		 * Remarks: Only for 2 bytes per pixel Image (otherwise undefined behaviour)
		 */
		void value2(uint8_t R4, uint8_t G4, uint8_t B4);
		
		// Operators 
		bool operator==(const pixel_iterator& it) const;
		bool operator!=(const pixel_iterator& it) const;
		bool operator<(const pixel_iterator& it) const;

	};

	pixel_iterator begin() const;
	pixel_iterator end() const;

	// Default constructor
	Image();

	// Create empty surface constructor
	Image(unsigned int width, unsigned int height, unsigned int depth);

	// SDL_Surface* move constructor
	// Remarks: given SDL_Surface will be attached in Image,
	//          but pointer to this surface will be 'nullptr' outside the class
	//          to prevent its deallocation outside class
	Image(SDL_Surface *moved_surface);

	// Copying surface
	Image(const SDL_Surface *copied_surface);

	// Copy constructor
	Image(const Image &);

	// Move constructor
	Image(Image &&);

	// Universal operator (copy and move assigment)
	Image& operator=(Image);

	// Destructor
	~Image();

	void printDetails(std::ostream &out = std::cout) const;

	/**
	 * @return pointer to constant SDL_Structure which
	 * allows to access data, but won't let to modify it
	 */
	const SDL_Surface* img() const;

	/**
	 * @param rendering context
	 * @return pointer to newly render texture from this surface
	 * Note: Surface is not freed or modified upon invoking that method
	 * @throws RuntimError on failure
	 */
	SDL_Texture* texture(SDL_Renderer *) const;

	/**
	 * @return number of pixels in horizontal line
	 */
	unsigned int width() const;

	/** 
	 * @return number of pixels in vertical line
	 */
	unsigned int height() const;
	
	/**
	 * @return number of bytes each pixel takes in memory
	 */
	unsigned int bpp() const;

	/**
	 * @return color depth of the image
	 */
	unsigned int depth() const;

	/**
	 * @return number of bytes needed to store pixel data in memory
	 */
	size_t size() const;

	/**
	 * Indicates wheter SDL_Surface structure is initialized or not
	 * @return bool true if so | false otherwise
	 */
	bool empty() const;
};

#endif // !IMAGE_H