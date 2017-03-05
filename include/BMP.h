#ifndef BMP_H
#define BMP_H

#include "ImageHandler.h"
class BMP : public ImageHandler
{
protected:
	virtual void store(const std::string &filename, const Image &image) const override;
	virtual Image recover(const std::string &filename) override;
public:
	virtual std::string extension() const override;
	BMP() = default;
	BMP(const ImageHandler &img);
	BMP(ImageHandler &&img);
	BMP& operator=(const ImageHandler &img);
	BMP& operator=(ImageHandler &&img);
};

#endif // !BMP_H