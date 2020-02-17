//
// Created by rewbycraft on 1/19/20.
//

#include <sixelping/receiver/metrics.h>
#include <sixelping/receiver/frames.h>
#include "sixelping/receiver/common.h"
#include "sixelping/receiver/application_configuration.h"
#include "sixelping/receiver/packets.h"
#include "sixelping/receiver/lcoremain.h"
#include <sixelping/exitcontrol.h>
#include <sixelping/logs.h>
#include <sixelping/traced_error.h>

namespace sixelping::receiver::lcore {
	
	int lcore_main(void *arg) {
		auto logger = sixelping::get_or_create_logger("lcoremain");
		try {
			auto *aconf = (struct app_config *) arg;
			auto lcore_id = static_cast<uint16_t>(rte_lcore_id());
			uint16_t lcore_index = static_cast<uint16_t>(rte_lcore_index(lcore_id));
			
			logger->info( "Starting core {}, index {}.", lcore_id, lcore_index);
			if (lcore_index == 0) {
				logger->info( "Core {} is working the metrics.", lcore_id);
				
				sixelping::receiver::metrics::run(aconf);
				sixelping::exit::make_exit();
			} else if (lcore_index == 1) {
				logger->info("Core {} is working the frames.", lcore_id);
				
				sixelping::receiver::frames::run(aconf);
				sixelping::exit::make_exit();
			} else if (lcore_index > 1 && lcore_index-1 <= aconf->ethdev.nb_rx_queues) {
				uint16_t queue_id = aconf->ethdev.first_rx_queue_id + lcore_index - 2;
				logger->info("Core {} is working RX queue {}.", lcore_id, queue_id);
				struct rte_mbuf *bufs[aconf->app.rx_burst_size];
				
				while (!sixelping::exit::should_exit()) {
					//Read packets from ethernet device
					const unsigned nb_rx = rte_eth_rx_burst(aconf->ethdev.port_id, queue_id, bufs, aconf->app.rx_burst_size);
					
					if (unlikely(nb_rx == 0)) {
						//rte_pause();
						continue;
					}
					
					for (uint16_t i = 0; i < nb_rx; i++) {
						sixelping::receiver::packets::process_packet(aconf, bufs[i]);
						nipktmfree(bufs[i]);
					}
					
				}
				sixelping::exit::make_exit();
			}
			
			logger->info("Exiting core {}.", lcore_id);
			return 0;
		} catch (const std::exception &e) {
			logger->error("Caught exception: {}", sixelping::traced::print(e));
			sixelping::exit::make_exit(1);
			return 1;
		}
		
	}
	
}
