//
// Created by rewbycraft on 2/13/20.
//

#ifndef SIXELPING_CPP_LOGS_H
#define SIXELPING_CPP_LOGS_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace sixelping {
	std::shared_ptr<spdlog::logger> get_or_create_logger(const std::string& name);
}

#endif //SIXELPING_CPP_LOGS_H
