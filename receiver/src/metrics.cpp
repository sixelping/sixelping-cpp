//
// Created by rewbycraft on 2/12/20.
//

#include <sixelping/timed_loop.h>
#include <prometheus/gauge.h>
#include <prometheus/counter.h>
#include "sixelping/receiver/metrics.h"
#include <sixelping/exitcontrol.h>
#include <sixelping/promutil.h>

namespace sixelping::receiver::metrics {
	void setup(struct app_config *aconf) {
		aconf->metrics.exposer = std::make_shared<prometheus::Exposer>(aconf->metrics.listen_address);
		aconf->metrics.registry = std::make_shared<prometheus::Registry>();
		aconf->metrics.exposer->RegisterCollectable(aconf->metrics.registry);
	}
	
	std::string format_v6(ipv6_addr_t _addr) {
		auto* addr = reinterpret_cast<uint8_t *>(&_addr);
		char str[64];
		sprintf(str, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7], addr[8],
		        addr[9], addr[10], addr[11], addr[12], addr[13], addr[14], addr[15]);
		return std::string(str);
	}
	
	void run(struct app_config *aconf) {
		auto& packets_received_counter = sixelping::promutil::counter_to_gauge(prometheus::BuildCounter().Name("receiver_packets_received").Register(*(aconf->metrics.registry)).Add({{"mac", aconf->ethdev.mac}}));
		auto& packets_sent_counter = sixelping::promutil::counter_to_gauge(prometheus::BuildCounter().Name("receiver_packets_sent").Register(*(aconf->metrics.registry)).Add({{"mac", aconf->ethdev.mac}}));
		auto& packets_dropped_counter = sixelping::promutil::counter_to_gauge(prometheus::BuildCounter().Name("receiver_packets_dropped").Register(*(aconf->metrics.registry)).Add({{"mac", aconf->ethdev.mac}}));
		auto& bytes_received_counter = sixelping::promutil::counter_to_gauge(prometheus::BuildCounter().Name("receiver_bytes_received").Register(*(aconf->metrics.registry)).Add({{"mac", aconf->ethdev.mac}}));
		auto& bytes_sent_counter = sixelping::promutil::counter_to_gauge(prometheus::BuildCounter().Name("receiver_bytes_sent").Register(*(aconf->metrics.registry)).Add({{"mac", aconf->ethdev.mac}}));
		auto& per_client = prometheus::BuildCounter()
			.Name("receiver_pings_received_per_client")
			.Labels({{"mac", aconf->ethdev.mac}})
			.Register(*(aconf->metrics.registry));
		sixelping::timed_loop("metrics", 1, [&]() {
			
			struct rte_eth_stats stats = {};
			uint16_t port = aconf->ethdev.port_id;
			
			if (!rte_eth_stats_get(port, &stats)) {
				packets_received_counter.Set(stats.ipackets);
				packets_sent_counter.Set(stats.opackets);
				packets_dropped_counter.Set(stats.imissed);
				bytes_received_counter.Set(stats.ibytes);
				bytes_sent_counter.Set(stats.obytes);
			}
			
			for (const auto& [k,v] : aconf->metrics.senders) {
				auto& metric = per_client.Add({{"ip", format_v6(k)}});
				sixelping::promutil::counter_to_gauge(metric).Set(v);
			}
			
			return !sixelping::exit::should_exit();
		});
	}
}
