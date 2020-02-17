//
// Created by rewbycraft on 1/17/20.
//

#ifndef SIXELPING_RECEIVER_DPDK_PIXELS_H
#define SIXELPING_RECEIVER_DPDK_PIXELS_H

#include "common.h"
#include "sixelping/receiver/application_configuration.h"
#include <vector>
#include <optional>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace sixelping::receiver::pixels {
	
	void setup(struct app_config *aconf);
	
	void handle_new_pixel(struct app_config *aconf, uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b);
	
	boost::shared_ptr<sixelping::Image<sixelping::rgba_pixel_t<uint16_t>>> swap_buffers(struct app_config *aconf);
}

#endif //SIXELPING_RECEIVER_DPDK_PIXELS_H
