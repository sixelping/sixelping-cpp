//
// Created by rewbycraft on 2/17/20.
//

#include <sixelping/promutil.h>

namespace sixelping::promutil {
	struct promhack_t {
		static const prometheus::MetricType metric_type{prometheus::MetricType::Counter};
		prometheus::Gauge gauge_;
	};
	
	prometheus::Gauge& counter_to_gauge(prometheus::Counter& counter) {
		return ((promhack_t*)(&counter))->gauge_;
	}
}