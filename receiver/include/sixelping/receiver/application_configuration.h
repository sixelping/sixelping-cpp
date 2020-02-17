//
// Created by rewbycraft on 1/17/20.
//

#ifndef SIXELPING_RECEIVER_DPDK_APPLICATION_CONFIGURATION_H
#define SIXELPING_RECEIVER_DPDK_APPLICATION_CONFIGURATION_H
#include "common.h"
#include <atomic>
#include <unordered_map>
#include <boost/smart_ptr/atomic_shared_ptr.hpp>
#include <sixelping/pixel.h>
#include <memory>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>

typedef unsigned __int128 ipv6_addr_t;

struct app_ethernet_config {
	uint8_t port_id;
	uint16_t first_rx_queue_id;
	uint16_t first_tx_queue_id;
	uint16_t nb_rx_queues;
	uint16_t nb_tx_queues;
	uint16_t rx_descs;
	uint16_t tx_descs;
	std::string mac;
};

struct app_metrics_config {
	std::string listen_address;
	std::unordered_map<ipv6_addr_t,std::atomic_uint64_t> senders;
	std::shared_ptr<prometheus::Exposer> exposer;
	std::shared_ptr<prometheus::Registry> registry;
};

struct app_pixels_config {
	unsigned width;
	unsigned height;
	boost::atomic_shared_ptr<sixelping::Image<sixelping::rgba_pixel_t<uint16_t>>> current_frame;
};

struct app_frames_config {
	std::string endpoint;
	double fps;
};

struct app_processing_config {
	uint16_t rx_burst_size;
	uint16_t process_burst_size;
};

struct app_config {
	struct app_ethernet_config ethdev;
	struct app_pixels_config pixels;
	struct app_processing_config app;
	struct app_metrics_config metrics;
	struct app_frames_config frames;
	struct rte_mempool *mbuf_pool;
	uint8_t ipv6_address[16];
	uint8_t ipv6_icmp_prefix[16];
	uint8_t ipv6_icmp_prefix_cidr;
};

#endif //SIXELPING_RECEIVER_DPDK_APPLICATION_CONFIGURATION_H
