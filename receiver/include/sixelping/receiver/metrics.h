//
// Created by rewbycraft on 2/12/20.
//

#ifndef SIXELPING_CPP_METRICS_H
#define SIXELPING_CPP_METRICS_H

#include <sixelping/receiver/application_configuration.h>

namespace sixelping::receiver::metrics {
	void setup(struct app_config *aconf);
	void run(struct app_config *aconf);
}

#endif //SIXELPING_CPP_METRICS_H
