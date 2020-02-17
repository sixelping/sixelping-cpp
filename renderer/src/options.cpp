//
// Created by rewbycraft on 2/10/20.
//

#include "sixelping/renderer/options.h"
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

namespace sixelping::renderer::options {
	struct options_t opts;
	
	const struct options_t& get() {
		return opts;
	}
	
	int parse(int& argc, char* argv[]) {
		// Declare the supported options.
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("width", po::value<int>()->default_value(1920), "width")
			("height", po::value<int>()->default_value(1080), "height")
			("ingress_port", po::value<unsigned short>()->default_value(50052), "Listen port for ingress.")
			("egress_port", po::value<unsigned short>()->default_value(50053), "Listen port for egress.")
			("metrics_port", po::value<unsigned short>()->default_value(50000), "Listen port for metrics.")
			("composit_fps", po::value<double>()->default_value(30.0), "Composition FPS")
			("egress_fps", po::value<double>()->default_value(30.0), "Egress FPS")
			("fade_out", po::value<double>()->default_value(10.0), "Fade out duration")
			("overlay_fps", po::value<double>()->default_value(1.0), "Overlay fps")
			("logo", po::value<std::string>(), "Logo path")
			("font", po::value<std::string>()->default_value("Ubuntu-Mono"), "Font to use (imagemagick name)")
			("font_size", po::value<unsigned>()->default_value(13), "Font size in points")
			("prom", po::value<std::string>(), "Prometheus endpoint")
			;
		
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
		
		if (vm.count("help")) {
			std::cout << desc << "\n";
			return 1;
		}

#define read_option(tp, name, req) {\
			if (vm.count(#name) || vm[#name].defaulted()) {\
				opts.name = vm[#name].as<tp>();\
			} else if (req) {\
				std::cerr << "Option \"" << #name << "\" is missing!" << std::endl;\
				return 1;\
			}\
		}
		
		read_option(int, width, true);
		read_option(int, height, true);
		read_option(unsigned short, ingress_port, true);
		read_option(unsigned short, egress_port, true);
		read_option(unsigned short, metrics_port, true);
		read_option(double, composit_fps, true);
		read_option(double, egress_fps, true);
		read_option(double, fade_out, true);
		read_option(double, overlay_fps, true);
		read_option(std::string, logo, false);
		read_option(std::string, font, true);
		read_option(unsigned, font_size, true);
		read_option(std::string, prom, false);
		
	        return 0;
	}
}
