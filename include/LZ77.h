#ifndef LZ77_H
#define LZ77_H
#include "Image.h"
#include <map>
#include <fstream>

class LZ77
{
private:
	// size of search buffer
	const unsigned int s_buff_size;

	// size of lookahead buffer
	const unsigned int la_buff_size;

	// search buffer
	std::map <int, uint8_t> s_buff;

	// lookahead buffer
	std::map <int, uint8_t> la_buff;

	//encoding functions
	short create_code(std::ofstream &ofile);
	void load_la_buff(std::array<uint8_t, 3> &color, Image::pixel_iterator &current, const Image::pixel_iterator &end, short &what_color, int start = 0, int addition = 0);
	void insert_elements_to_s_buff(short max_length, int &it);

	/**
	 * Deletes n first elements from the map
	 * @param map<int, uint8_t> buffer 
	 * @param size_t n
	 */
	void popFront(std::map<int, uint8_t> &buffer, size_t n);

	//decoding functions
	short read_code(uint8_t code);
	void put_elements_into_s_buff(short length);

public:
	LZ77();
	void encode(std::ofstream&, const Image &);
	void decode(std::ifstream&, Image &);
};

#endif // !LZ77_H
