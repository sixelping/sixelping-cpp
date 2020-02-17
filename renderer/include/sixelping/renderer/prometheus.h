//
// Created by rewbycraft on 2/16/20.
//

#ifndef SIXELPING_CPP_PROMETHEUS_H
#define SIXELPING_CPP_PROMETHEUS_H
#include <optional>
#include <string>

namespace sixelping::renderer::prometheus {
	std::optional<double> query(std::string query);
}

#endif //SIXELPING_CPP_PROMETHEUS_H
