#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <utility>
#include "SDL_Local.h"
#include "Huffman.h"
#include "BMP.h"
#include "RGB12.h"
#include "LZ77.h"
#include "CText.h"
#include "RuntimeError.h"

using namespace std;

///----UTILITIES FOR TESTS------
/**
 * @usage
 *	auto begin = std::chrono::steady_clock::now();
 *	auto end = std::chrono::steady_clock::now();
 *	showDuration(begin, end, "Test name");
 *
 */
void showDuration(std::chrono::steady_clock::time_point begin, std::chrono::steady_clock::time_point end, const char *cstring = "Measured", std::ostream& o = std::cout)
{
	// Default (10^-3)
	char magnitude_prefix = 'm';
	long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	
	// Change to microseconds (10^-6)
	if (duration < 10)
	{
		magnitude_prefix = 'u';
		duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
	}

	// Change to nanoseconds (10^-9)
	if(duration < 10)
	{
		magnitude_prefix = 'n';
		duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
	}

	std::ostringstream os;
	os << duration << ' ' << magnitude_prefix << 's';

	o << cstring << " duration time: " << CText(os.str(), CText::Color::GREEN) << std::endl;
}
///-----------------------------

void test_BMPHandler()
{
	BMP bmp;
	bmp.load("test/smalltest_24bit.bmp");
	bmp.preview(true);
	bmp.save("test/bmp_handler_test");
	bmp.load("test/bmp_handler_test.bmp");
	bmp.preview(true);
	
	// copy constructor;
	BMP bmp2(bmp);
	bmp2.preview();

	// move constructor
	BMP bmp3(std::move(bmp2));
	bmp3.preview();
	bmp2.preview(); //should fail

	// Copy assigment
	ImageHandler *bmp4 = new BMP();
	bmp4->load("test/smalltest_24bit.bmp");
	*bmp4 = bmp3;
	bmp4->preview();

	// Move assigment
	BMP bmp5;
	bmp5.load("test/smalltest_24bit.bmp");
	bmp5 = std::move(*reinterpret_cast<BMP *>(bmp4));
	bmp5.preview();
	bmp4->preview(); //should fail
	delete bmp4;

}

void test_RGB12Handler()
{
	BMP bmp;
	bmp.load("test/smalltest_8bit.bmp");
	bmp.preview();

	// Convert construct
	RGB12 rgb(bmp);
	rgb.preview();
	bmp.preview();

	// Copy construct
	RGB12 rgb2(rgb);
	rgb2.toGrayScale();
	rgb2.preview();

	// Move construct
	RGB12 rgb3(std::move(rgb2));
	rgb3.toGrayScale();
	rgb3.preview();
	rgb2.preview(true); //should fail

	// Initize with convert construct to force object to be constructed then
	// Copy assigment
	RGB12 rgb4(bmp);
	rgb4 = rgb3;
	rgb4.toGrayScale();
	rgb4.preview();
	rgb3.preview();

	// Initize with convert construct to force object to be constructed then
	// Move assigment
	RGB12 rgb5(bmp);
	rgb5 = std::move(rgb4);
	rgb5.toGrayScale();
	rgb5.preview();
	rgb4.preview(true); // should fail
}

void test_BitDensity(const std::string &test)
{
	BMP bmp;
	bmp.load(test);
	bmp.preview();

	// Save
	RGB12 rgb(bmp, RGB12::Algorithm::BitDensity);

	auto begin = std::chrono::steady_clock::now();
	rgb.save("test/image");
	auto end = std::chrono::steady_clock::now();
	showDuration(begin, end, "Encoded BitDenisty");
	

	// Load
	RGB12 rgb2(RGB12::Algorithm::Huffman);

	begin = std::chrono::steady_clock::now();
	rgb2.load("test/image.rgb12");
	end = std::chrono::steady_clock::now();
	showDuration(begin, end, "BitDenisty decoded");

	rgb2.preview();

}

void test_Huffman(const std::string &test)
{
	BMP bmp;
	bmp.load(test);
	bmp.preview();

	/// ENCODING
	RGB12 rgb(bmp, RGB12::Algorithm::Huffman);
	rgb.preview();
	
	auto begin = std::chrono::steady_clock::now();
	rgb.save("test/huffman");
	auto end = std::chrono::steady_clock::now();
	showDuration(begin, end, "Huffman fully encode");
	/**
	 * Records:
	 * test/rgbcube.bmp - 43ms (g++ -03/Linux/x64/notebook 2-core i7) 
	 * test/rgbcube.bmp - 123ms (VS Release/x64/notebook 2-core i7) 
	 * test/test.bmp - 8ms (g++ -O3/Linux/x64/notebook 2-core i7) 
	 * test/test.bmp - 30ms (VS Release/x64/notebook 2-core i7)
	 * test/smalltest_8bit.bmp - 248us (g++ -03/Linux/x64/notebook 2-core i7)
     * test/smalltest_8bit.bmp - 30ms (VS Release/x64/notebook 2-core i7)
	 * test/smalltest_24bit.bmp - 28ms (VS Release/x64/notebook 2-core i7)
 	 */

	/// DECODING
	RGB12 rgb2;

	begin = std::chrono::steady_clock::now();
	rgb2.load("test/huffman.rgb12");
	end = std::chrono::steady_clock::now();
	showDuration(begin, end, "Huffman fully decoded");
	/**
	 * Records:
	 * test/rgbcube.bmp - 982ms (VS Release/x64/notebook 2-core i7)
	 * test/rgbcube.bmp - 1482ms (g++ -03/Linux/x64/notebook 2-core i7) (!????)
	 * test/test.bmp - 10ms (g++ -O3/Linux/x64/notebook 2-core i7)
     * test/test.bmp - 65ms (VS Release/x64/notebook 2-core i7)
     * test/smalltest_8bit.bmp - 250us (g++ -03/Linux/x64/notebook 2-core i7)
	 * test/smalltest_8bit.bmp - 30ms (VS Release/x64/notebook 2-core i7)
	 * test/smalltest_24bit.bmp - 36ms (VS Release/x64/notebook 2-core i7)
 	 */

	rgb2.preview();
}

void test_LZ77(const std::string &test)
{
	BMP bmp;
	bmp.load(test);
	bmp.preview();

	RGB12 rgb(bmp, RGB12::Algorithm::LZ77);
	rgb.preview();

	auto begin = std::chrono::steady_clock::now();
	rgb.save("test/lz77");
	auto end = std::chrono::steady_clock::now();
	showDuration(begin, end, "LZ77 fully encoded");
	
	RGB12 rgb2;
	begin = std::chrono::steady_clock::now();
	rgb2.load("test/lz77.rgb12");
	end = std::chrono::steady_clock::now();
	showDuration(begin, end, "LZ77 decoded");
	
	rgb2.preview();

}

void test_Grey(const std::string &test)
{
	BMP bmp;
	bmp.load(test);

	// Test look
	RGB12 rgb(bmp);
	rgb.toGrayScale();
	rgb.preview();

	// Saving
	RGB12 grey_save(bmp, RGB12::Algorithm::GrayScale);

	auto begin = std::chrono::steady_clock::now();
	grey_save.save("test/grey_scaled");
	auto end = std::chrono::steady_clock::now();
	showDuration(begin, end, "Grey encoded");
	

	// Opening in different instance
	RGB12 grey;
	begin = std::chrono::steady_clock::now();
	grey.load("test/grey_scaled.rgb12");
	end = std::chrono::steady_clock::now();
	showDuration(begin, end, "Grey decoded");
	
	grey.preview();
}

void test_Image()
{
	BMP bmp, test;
	bmp.load("test/smalltest_24bit.bmp");

	test.image = bmp.image;
	bmp.preview();
	test.preview();

	{
		RGB12 temp;
		temp = bmp;
		temp.toGrayScale();
		bmp = std::move(temp);
	}
	test.image = std::move(bmp.image);
	bmp.preview(); // should fail
	test.preview();

	Image copied(test.image);
	test.image = copied;
	test.preview();

	Image moved(std::move(copied));
	test.image = std::move(copied);
	test.preview(); // should fail
	test.image = std::move(moved);
	test.preview();
}

void openCompressSaveBMP(const std::string &test)
{
	BMP bmp;
	bmp.load(test);
	{
		RGB12 rgb(std::move(bmp));
		rgb.save("test/big/big_upakowanie");
		rgb.algorithm = RGB12::Algorithm::Huffman;
		rgb.save("test/big/big_huffman");
		rgb.algorithm = RGB12::Algorithm::LZ77;
		rgb.save("test/big/big_lz77");
		rgb.algorithm = RGB12::Algorithm::GrayScale;
		rgb.save("test/big/big_gray");
	}

	{
		RGB12 rgb;
		BMP bmp;

		rgb.load("test/big/big_upakowanie.rgb12");
		bmp = std::move(rgb);
		bmp.save("test/big/big_upakowanie");

		rgb.load("test/big/big_huffman.rgb12");
		bmp = std::move(rgb); 
		bmp.save("test/big/big_huffman");
	
		rgb.load("test/big/big_lz77.rgb12");
		bmp = std::move(rgb); 
		bmp.save("test/big/big_lz77");

		rgb.load("test/big/big_gray.rgb12");
		bmp = std::move(rgb);
		bmp.save("test/big/big_gray");
	}
}

int main()
{
	std::string testImg;

    // Initialize SDL
    try
    {
        SDL sdl(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    }
    catch (const RuntimeError &err)
    {
        cerr << "Error while initializing SDL:  " << err.what() << endl;
        return 1;
    }


    cout << "Testing.." << endl;
	//test_Image();
	//test_BMPHandler();
	//test_RGB12Handler();

	///testImg = "test/big/big.bmp";
	//testImg = "test/wide.bmp";
	//testImg = "test/1x1.bmp";
	//testImg = "test/rgbcube.bmp";
	testImg = "test/test.bmp";
	//testImg = "test/smalltest_24bit.bmp";
	//testImg = "test/smalltest_8bit.bmp";

	/// Algs
	//test_BitDensity(testImg);
	//test_Huffman(testImg);
	test_LZ77(testImg);
	//test_Grey(testImg);
	//openCompressSaveBMP(testImg);

	return 0;
}
