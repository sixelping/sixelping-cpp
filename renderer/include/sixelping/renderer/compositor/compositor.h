//
// Created by rewbycraft on 2/10/20.
//

#ifndef SIXELPING_RENDERER_COMPOSITOR_H
#define SIXELPING_RENDERER_COMPOSITOR_H
#include <thread>
#include <sixelping/pixel.h>
#include <memory>
#include <boost/shared_ptr.hpp>

namespace sixelping::renderer::compositor {
	void init();
	
	std::thread start();
	
	boost::shared_ptr<sixelping::Image<>> get_last_image();
	
	std::chrono::milliseconds get_last_loop_time();
}

#endif //SIXELPING_RENDERER_COMPOSITOR_H
