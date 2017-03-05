#include "LZ77.h"
#include <vector>

#ifdef _DEBUG
#include <iostream>
#endif

LZ77::LZ77()

	//size of search buffer
	:s_buff_size(17), 

	// size of lookahead buffer
	la_buff_size(20)
{}



//------------------------------ENCODING------------------------------

/** 
 * @param opened output stream
 * @param vaild Image to save
 */
void LZ77::encode(std::ofstream &ofile, const Image &image)
{
#ifdef _DEBUG
	std::cout<<"\n=== LZ77 COMPRESSION ==="<<std::endl;
#endif
	
	// Image iterator
	auto const img_end = image.end();
	auto pixel_it = image.begin();

	// Loading first pixel to initialization variables
	std::array<uint8_t, 3> color = pixel_it.rgb2();
	++pixel_it;
	for (auto &c : color)
		c >>= 4;

	// Saving first subpixel
	ofile.write(reinterpret_cast<const char*>(&color[0]), sizeof(color[0]));

	// Initialization of search buffer 	
	for (int i = 0; i < static_cast<int>(s_buff_size); ++i)
		s_buff.insert(std::make_pair(i, color[0]));

	// Variable pointing the subpixel
	short what_color = 1;

	// The first loading data to la_buff 
	load_la_buff(color, pixel_it, img_end, what_color);

	// Variables to search the longest sequence
	short max_length = 0;

	// Main part of algorithm - loading data and coding
	int it = 0;
	while (la_buff.size() > 0)
	{
		load_la_buff(color, pixel_it, img_end, what_color, (it - max_length + 1), la_buff_size);
		max_length = create_code(ofile);
		insert_elements_to_s_buff(max_length, it);

		popFront(s_buff, max_length);
		popFront(la_buff, max_length);
	}

//	la_buff.clear();
	s_buff.clear();
#ifdef _DEBUG
	std::cout<<"\n=== LZ77 COMPRESSION DONE ==="<<std::endl;
#endif
}


/** 
 * @param opened output stream
 */
short LZ77::create_code(std::ofstream &ofile)
{
	auto s_begin = s_buff.begin();
	auto s_end = s_buff.end();
	auto la_begin = la_buff.begin();
	auto la_end = la_buff.end();

	short length = 0, 
		max_length = 1, 
		position = 0;
	bool stop = false;
		
	// Searching the longest sequence
	for(auto k = s_end; k != s_begin && !stop; --k)
	{
		auto count = k;
		auto temp = la_begin;
		while (count != s_end && temp != la_end && count->second == temp->second)
		{
			if (length >= 9)
			{
				stop = true;
				break;
			}
			++count;
			++temp;
			++length;
		}

		if (max_length < length)
		{
			max_length = length;
			position = k->first;	
		}

		length = 0;
	}

	// Creating code of subpixels
	if (max_length == 1)
		ofile.write(reinterpret_cast<const char*>(&la_begin->second), sizeof(la_begin->second));
	else
	{
		uint8_t code = 128 | (max_length - 2) << 4 | (position - s_begin->first);
		ofile.write(reinterpret_cast<const char*>(&code), sizeof(code));
	}

	return max_length;
}

void LZ77::load_la_buff(std::array<uint8_t, 3> & color, Image::pixel_iterator & current, const Image::pixel_iterator & end, short & what_color, int it, int addition)
{
	while (la_buff.size() < la_buff_size && (current < end || what_color < 3))
	{
		if (what_color < 3)
		{
			la_buff.insert(std::make_pair(addition + it, color[what_color]));
			++it;
			++what_color;
		}
		else
		{
			color = current.rgb2();
			++current;
			for (auto &c : color)
				c >>= 4;
			what_color = 0;
		}
	}
}


/** 
 * @param size of the longest sequence
 * @param iterator pointing the beginning of search buffer
 */
void LZ77::insert_elements_to_s_buff(short max_length, int &it)
{
	// Inserting elements into search buffer
	auto la_begin = la_buff.begin();
	while (max_length > 0)
	{
		s_buff.insert(std::make_pair(static_cast<int>(s_buff_size + it), la_begin->second));
		++la_begin;
		++it;
		--max_length;
	}
}

void LZ77::popFront(std::map<int, uint8_t> & buffer, size_t n)
{
	auto end = buffer.begin();
	//n = (n < buffer.size()) ? n : buffer.size();		// security condition for debugging
	std::advance(end, n);
	buffer.erase(buffer.begin(), end);
}



//------------------------------DECODING------------------------------

/**
 * @param opened input stream
 * @param allocated empty Image with proper width/etc
 */
void LZ77::decode(std::ifstream &ifile, Image &image)
{
#ifdef _DEBUG
	std::cout << "\n=== LZ77 DECOMPRESSION ===" << std::endl;
#endif

	//variables to decoding
	short length = 0;

	//colors of the one pixel
	uint8_t color[3];

	//variable pointing subpixel
	short what_color = 1;

	// Load whole file at once
	std::vector<char> codes = std::vector<char>(std::istreambuf_iterator<char>(ifile), std::istreambuf_iterator<char>());
	codes.push_back(0);
	auto code = codes.begin();

	// Fill search buffer with first byte
	color[0] = static_cast<uint8_t>(*code) << 4;
	++code;
	for (int i = 0; i < static_cast<int>(s_buff_size); ++i)
		s_buff.insert(std::make_pair(i, color[0]));

	auto pixel_it = image.begin();
	auto codes_end = codes.end();

	while(code < codes_end)
	{
		length = read_code(static_cast<uint8_t>(*code));
		put_elements_into_s_buff(length);
		popFront(s_buff, length);

		for (const auto p : la_buff)
		{
			if (what_color >= 3)
			{
				pixel_it.value2(color[0], color[1], color[2]);
				++pixel_it;
				what_color = 0;
			}

			color[what_color] = p.second;
			++what_color;
		}
		la_buff.clear();
		++code;
	}

	s_buff.clear();
#ifdef _DEBUG
	std::cout << "\n=== LZ77 DECOMPRESSION DONE ===" << std::endl;
#endif
}


/** 
 * @param readed code
 */
short LZ77::read_code(uint8_t code)
{
	auto s_first = s_buff.begin()->first;

	uint8_t first_bit = code >> 7;
	short length;
	//checking what we have- one subpixel or sequence
	if (first_bit)
	{
		//decoding sequence of subpixels in one byte
		length = (static_cast<int>(code) >> 4) - 8;
		short position = static_cast<int>(code) - 128 - (length << 4);
		length += 2;

		short it = 0;
		while (it < length)
		{
			la_buff.insert(std::make_pair(s_first + it, s_buff[s_first + position + it]));
			++it;
		}
	}
	else
	{
		//decoding one subpixel
		code = code << 4;
		la_buff.insert(std::make_pair(0, code));
		length = 1;
	}
	return length;
}


/** 
 * @param length of sequence of subpixels or one subpixel (=1)
 */
void LZ77::put_elements_into_s_buff(short length)
{
	//inserting elements to search buffer
	auto s_first = s_buff.begin()->first;
	auto la_begin = la_buff.begin();

	short it = 0;
	while (length > 0)
	{
		s_buff.insert(std::make_pair(s_first + s_buff_size + it, la_begin->second));
		++la_begin;
		++it;
		--length;
	}
}
