//
// Created by rewbycraft on 2/6/20.
//

#ifndef SIXELPING_RENDERER_COLLECTOR_H
#define SIXELPING_RENDERER_COLLECTOR_H
#include <vector>
#include <cstdint>
#include <chrono>
#include <sixelping/pixel.h>

namespace sixelping::renderer::ingress::collector {
	
	void init();
	
	void handle_new_delta_image(const sixelping::Image<sixelping::rgba_pixel_t<uint16_t>> &image);
	
	void write_current_image(sixelping::Image<>& output_image, std::chrono::milliseconds fade_out, unsigned x_offset, unsigned y_offset, unsigned width, unsigned height);
	
}

#endif //SIXELPING_RENDERER_COLLECTOR_H
