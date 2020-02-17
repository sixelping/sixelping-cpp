//
// Created by rewbycraft on 2/10/20.
//

#ifndef SIXELPING_RENDERER_OVERLAY_H
#define SIXELPING_RENDERER_OVERLAY_H
#include <thread>
#include <sixelping/pixel.h>
#include <memory>
#include <boost/shared_ptr.hpp>

namespace sixelping::renderer::compositor::overlay {
	void init();
	
	std::thread start();
	
	void write_current_image(sixelping::Image<> &output_image, unsigned x_offset, unsigned y_offset, unsigned width, unsigned height);
}

#endif //SIXELPING_RENDERER_OVERLAY_H
