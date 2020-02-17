//
// Created by rewbycraft on 1/17/20.
//

#include "sixelping/receiver/pixels.h"
#include <boost/smart_ptr.hpp>
#include <iostream>

namespace sixelping::receiver::pixels {
	
	boost::shared_ptr<sixelping::Image<sixelping::rgba_pixel_t<uint16_t>>> create_new_image(struct app_config *aconf) {
		return boost::make_shared<sixelping::Image<sixelping::rgba_pixel_t<uint16_t>>>(aconf->pixels.width, aconf->pixels.height);
	}
	
	void setup(struct app_config *aconf) {
		aconf->pixels.current_frame.store(create_new_image(aconf));
	}
	
	void handle_new_pixel(struct app_config *aconf, uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {
		if (unlikely(x >= aconf->pixels.width || y >= aconf->pixels.height))
			return;
		
		auto& pix = aconf->pixels.current_frame.load()->getPixel(x, y);
		
		pix.r += r;
		pix.g += g;
		pix.b += b;
		pix.a++;
	}
	
	boost::shared_ptr<sixelping::Image<sixelping::rgba_pixel_t<uint16_t>>> swap_buffers(struct app_config *aconf) {
		auto current_image = aconf->pixels.current_frame.load();
		aconf->pixels.current_frame.store(create_new_image(aconf));
		
		return current_image;
	}
	
}