//
// Created by rewbycraft on 2/17/20.
//

#ifndef SIXELPING_CPP_PROMUTIL_H
#define SIXELPING_CPP_PROMUTIL_H

#include <prometheus/counter.h>
#include <prometheus/gauge.h>

namespace sixelping::promutil {
	prometheus::Gauge& counter_to_gauge(prometheus::Counter& counter);
}

#endif //SIXELPING_CPP_PROMUTIL_H
