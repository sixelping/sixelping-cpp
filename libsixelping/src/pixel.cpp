//
// Created by rewbycraft on 2/10/20.
//

#include "sixelping/pixel.h"
namespace sixelping {
	
	template<typename pixel>
	Image<pixel>::Image(unsigned int width, unsigned int height) : data(), width(width), height(height) {
		data.resize(width*height);
	}
	
	template<typename pixel>
	std::vector<pixel> &Image<pixel>::getData() {
		return data;
	}
	
	template<typename pixel>
	unsigned int Image<pixel>::getWidth() const {
		return width;
	}
	
	template<typename pixel>
	unsigned int Image<pixel>::getHeight() const {
		return height;
	}
	
	template<typename pixel>
	const std::vector<pixel> &Image<pixel>::getData() const {
		return data;
	}
	
	template class Image<rgb_pixel_t<uint8_t>>;
	template class Image<rgba_pixel_t<uint8_t>>;
	template class Image<rgba_pixel_t<uint16_t>>;
}
