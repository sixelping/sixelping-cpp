//
// Created by rewbycraft on 2/17/20.
//

#ifndef SIXELPING_CPP_METRICS_H
#define SIXELPING_CPP_METRICS_H

#include <prometheus/registry.h>

namespace sixelping::renderer::metrics {
	void init();
	::prometheus::Registry& get_registry();
}

#endif //SIXELPING_CPP_METRICS_H
