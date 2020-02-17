//
// Created by rewbycraft on 1/17/20.
//

#ifndef SIXELPING_RECEIVER_DPDK_ETHDEV_H
#define SIXELPING_RECEIVER_DPDK_ETHDEV_H
#include "sixelping/receiver/application_configuration.h"

namespace sixelping::receiver::ethdev {
	
	void probe_ethernet_device(struct app_config *aconf);
	
	void setup_ethernet_device(struct app_config *aconf);
	
	uint8_t determine_ethernet_port_id();
}

#endif //SIXELPING_RECEIVER_DPDK_ETHDEV_H
