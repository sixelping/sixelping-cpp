//
// Created by rewbycraft on 1/17/20.
//
#include <sixelping/receiver/common.h>
#include <sixelping/traced_error.h>
#include <sixelping/receiver/exceptions.h>
#include "sixelping/receiver/ethdev.h"
#include <sixelping/logs.h>
#include <boost/format.hpp>

namespace sixelping::receiver::ethdev {
	
	void probe_ethernet_device(struct app_config *aconf) {
		auto logger = sixelping::get_or_create_logger("ethdev-probe");
		logger->info("Querying ethernet device...");
		//Determine the ethernet port to use
		aconf->ethdev.port_id = determine_ethernet_port_id();
		
		struct rte_eth_dev_info dev_info = {};
		if (rte_eth_dev_info_get(aconf->ethdev.port_id, &dev_info))
			throw_with_trace(sixelping::receiver::exceptions::dpdk_exception("Unable to fetch interface information."));

		aconf->ethdev.first_rx_queue_id = 0;
		aconf->ethdev.first_tx_queue_id = 0;
		if (aconf->ethdev.nb_rx_queues <= 0)
			aconf->ethdev.nb_rx_queues = dev_info.max_rx_queues;
		if (aconf->ethdev.nb_tx_queues <= 0)
			aconf->ethdev.nb_tx_queues = dev_info.max_tx_queues;
		aconf->ethdev.nb_rx_queues = std::min(aconf->ethdev.nb_rx_queues, dev_info.max_rx_queues);
		aconf->ethdev.nb_tx_queues = std::min(aconf->ethdev.nb_tx_queues, dev_info.max_tx_queues);
		
		logger->info("Using {} rx queues and {} tx queues.", aconf->ethdev.nb_rx_queues, aconf->ethdev.nb_tx_queues);
		
		aconf->ethdev.rx_descs = dev_info.rx_desc_lim.nb_max;
		aconf->ethdev.tx_descs = dev_info.tx_desc_lim.nb_max;
		
		if (rte_eth_dev_adjust_nb_rx_tx_desc(aconf->ethdev.port_id, &aconf->ethdev.rx_descs, &aconf->ethdev.tx_descs))
			throw_with_trace(sixelping::receiver::exceptions::dpdk_exception("Cannot adjust rx/tx params."));
		
		logger->info("Max nb_rx_descriptors: {}", aconf->ethdev.rx_descs);
		logger->info("Max nb_tx_descriptors: {}", aconf->ethdev.tx_descs);

		//Use half the buffer space per poll.
		//This way we shouldn't drop anything when the buffer fills up.
		aconf->app.rx_burst_size = static_cast<uint16_t>(aconf->ethdev.rx_descs / 2);
		aconf->app.process_burst_size = static_cast<uint16_t>(aconf->ethdev.rx_descs / 2);
		//static_cast<uint16_t>(aconf->app.rx_burst_size / std::max((rte_lcore_count() - aconf->ethdev.nb_rx_queues - 1), (unsigned)1));
		
		logger->info("RX burst size: {}", aconf->app.rx_burst_size);
		logger->info("Process burst size: {}", aconf->app.process_burst_size);

		logger->info("Done querying ethernet device!");
	}
	
	void setup_ethernet_device(struct app_config *aconf) {
		auto logger = sixelping::get_or_create_logger("ethdev");
		
		logger->info("Starting ethernet device...");
		
		logger->info("Configuring device...");
		struct rte_eth_conf port_conf = {};
		port_conf.rxmode.max_rx_pkt_len = RTE_ETHER_MAX_LEN;
		port_conf.rxmode.mq_mode = ETH_MQ_RX_RSS;
		int ret;
		uint16_t q;
		
		/* Configure the Ethernet device. */
		ret = rte_eth_dev_configure(aconf->ethdev.port_id, aconf->ethdev.nb_rx_queues, aconf->ethdev.nb_tx_queues, &port_conf);
		if (ret != 0)
			throw_with_trace(sixelping::receiver::exceptions::dpdk_exception("Failed to configure device"));
		
		logger->info("Setting up queues...");
		
		/* Allocate and set up 1 RX queue per Ethernet port. */
		for (q = aconf->ethdev.first_rx_queue_id; q < aconf->ethdev.nb_rx_queues; q++) {
			ret = rte_eth_rx_queue_setup(aconf->ethdev.port_id, q, aconf->ethdev.rx_descs, static_cast<unsigned int>(rte_eth_dev_socket_id(aconf->ethdev.port_id)),
			                             NULL,
			                             aconf->mbuf_pool);
			if (ret < 0)
				throw_with_trace(sixelping::receiver::exceptions::dpdk_exception( boost::str(boost::format("Failed to setup rx queue %d") % q)));
		}
		
		/* Allocate and set up 1 TX queue per Ethernet port. */
		for (q = aconf->ethdev.first_tx_queue_id; q < aconf->ethdev.nb_tx_queues; q++) {
			ret = rte_eth_tx_queue_setup(aconf->ethdev.port_id, q, aconf->ethdev.tx_descs, static_cast<unsigned int>(rte_eth_dev_socket_id(aconf->ethdev.port_id)),
			                             NULL);
			if (ret < 0)
				throw_with_trace(sixelping::receiver::exceptions::dpdk_exception(boost::str(boost::format("Failed to setup tx queue %d") % q)));
		}
		
		logger->info("Starting device...");
		/* Start the Ethernet port. */
		ret = rte_eth_dev_start(aconf->ethdev.port_id);
		if (ret < 0)
			throw_with_trace(sixelping::receiver::exceptions::dpdk_exception("Failed to start device"));
		
		/* Enable RX in promiscuous mode for the Ethernet device. */
		rte_eth_promiscuous_enable(aconf->ethdev.port_id);
		rte_eth_allmulticast_enable(aconf->ethdev.port_id);
		
		char mac[64];
		struct rte_ether_addr eth = {};
		rte_eth_macaddr_get(aconf->ethdev.port_id, &eth);
		rte_ether_format_addr(mac, 64, &eth);
		aconf->ethdev.mac = std::string(mac);
		
		logger->info("Started ethernet device!");
	}
	
	uint8_t determine_ethernet_port_id() {
		auto logger = sixelping::get_or_create_logger("ethdev");
		
		if (rte_eth_dev_count_avail() == 0) {
			throw_with_trace(sixelping::receiver::exceptions::dpdk_exception("No ethernet devices available!"));
		}
		
		logger->info("Ethernet devices:");
		for (uint8_t i = 0; i < rte_eth_dev_count_avail(); i++) {
			char mac[64];
			struct rte_ether_addr addr = {};
			struct rte_eth_link link = {};
			
			if (rte_eth_macaddr_get(i, &addr)) {
				logger->info(" - Device {}: Unable to get mac address.", i);
				continue;
			}
			
			rte_ether_format_addr(mac, 64, &addr);
			
			if (rte_eth_link_get(i, &link)) {
				logger->info(" - Device {}: Unable to get link.", i);
				continue;
			}
			
			logger->info(" - Device {}:\tMAC: {}\tStatus: {}\tSpeed: {} mbps\tDuplex: {}", i, mac,
			        (link.link_status) ? "UP" : "DOWN", link.link_speed,
			        (link.link_duplex) ? "HALF" : "FULL");
		}
		
		if (rte_eth_dev_count_avail() == 1) {
			logger->info("Only one ethernet device available, selecting ethernet device 0.");
			return 0;
		}
		
		throw_with_trace(sixelping::receiver::exceptions::dpdk_exception("NOT IMPLEMENTED: MULTIPLE NICS, USING NIC 0"));
		return 0;
	}
	
}