//
// Created by rewbycraft on 2/10/20.
//

#ifndef SIXELPING_RENDERER_OPTIONS_H
#define SIXELPING_RENDERER_OPTIONS_H

#include <string>

namespace sixelping::renderer::options {
	struct options_t {
		unsigned width, height;
		unsigned short ingress_port, egress_port, metrics_port;
		double composit_fps, fade_out, egress_fps, overlay_fps;
		std::string logo, font, prom;
		unsigned font_size;
	};
	
	const struct options_t& get();
	
	int parse(int& argc, char* argv[]);
}


#endif //SIXELPING_RENDERER_OPTIONS_H
