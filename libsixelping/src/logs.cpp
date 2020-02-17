//
// Created by rewbycraft on 2/13/20.
//
#include <sixelping/logs.h>

namespace sixelping {
	std::mutex m;
	
	std::shared_ptr<spdlog::logger> get_or_create_logger(const std::string& name) {
		std::lock_guard<std::mutex> guard(m);
		auto l = spdlog::get(name);
		if (!l) {
			l = spdlog::stdout_color_mt(name);
		}
		return l;
	}
}

