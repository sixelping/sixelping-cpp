#include "sixelping/receiver/common.h"
#include "sixelping/receiver/application_configuration.h"
#include "sixelping/receiver/ethdev.h"
#include "sixelping/receiver/pixels.h"
#include "sixelping/receiver/lcoremain.h"
#include <sixelping/receiver/frames.h>
#include <sixelping/receiver/metrics.h>
#include <arpa/inet.h>
#include <boost/program_options.hpp>
#include <iostream>
#include <sixelping/exitcontrol.h>
#include <sixelping/traced_error.h>
#include <sixelping/receiver/exceptions.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace po = boost::program_options;

uint16_t hex_to_int(const char c) {
	if ('0' <= c && c <= '9') {
		return static_cast<uint8_t>(c - '0');
	}
	if ('a' <= c && c <= 'f') {
		return static_cast<uint8_t>(c - 'a');
	}
	if ('A' <= c && c <= 'F') {
		return static_cast<uint8_t>(c - 'A');
	}
	return 0xFF;
}

void parse_ipv6_address(const char *_str, uint8_t *addr, uint8_t *cidr) {
	char *str = (char*)_str;
	char *sptr = strchr(str, '/');
	size_t len = strlen(str);
	
	if (sptr != nullptr) {
		len = sptr - str;
		sptr++;
		*cidr = static_cast<uint8_t>(std::atoi(sptr));
	}
	char tstr[len + 1];
	strncpy(tstr, str, len);
	tstr[len] = '\0';
	
	struct in6_addr a = {};
	if (inet_pton(AF_INET6, tstr, &a) != 1) {
		rte_panic("Invalid ipv6 address.\n");
	}
	
	memcpy(addr, a.s6_addr, 16 * sizeof(uint8_t));
}

int populate_aconf(struct app_config *aconf, int argc, char *argv[]) {
	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("width", po::value<unsigned>()->default_value(1920), "width")
		("height", po::value<unsigned>()->default_value(1080), "height")
		("fps", po::value<double>()->default_value(30.0), "Emission FPS")
		("renderer", po::value<std::string>()->required(), "Renderer address")
		("prometheus", po::value<std::string>()->default_value("[::]:40000"), "Prometheus listen address")
		("rxqueues", po::value<unsigned>()->default_value(rte_lcore_count() - 2), "Renderer address")
		("address", po::value<std::string>()->required(), "Lan address")
		("prefix", po::value<std::string>()->required(), "ICMP prefix")
		;
	
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	
	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
	}
	
	aconf->pixels.width = vm["width"].as<unsigned>();
	aconf->pixels.height = vm["height"].as<unsigned>();
	aconf->frames.fps = vm["fps"].as<double>();
	aconf->frames.endpoint = vm["renderer"].as<std::string>();
	aconf->metrics.listen_address = vm["prometheus"].as<std::string>();
	aconf->ethdev.nb_rx_queues = vm["rxqueues"].as<unsigned>();
	parse_ipv6_address(vm["prefix"].as<std::string>().c_str(), aconf->ipv6_icmp_prefix, &aconf->ipv6_icmp_prefix_cidr);
	parse_ipv6_address(vm["address"].as<std::string>().c_str(), aconf->ipv6_address, nullptr);
	
	return 0;
}

int main(int argc, char *argv[]) {
	auto logger = spdlog::stdout_color_mt("main");
	try {
		int ret;
		struct app_config aconf = {};
		
		logger->info("Initializing DPDK...");
		//Initialize DPDK EAL
		ret = rte_eal_init(argc, argv);
		if (ret < 0)
			throw_with_trace(sixelping::receiver::exceptions::dpdk_exception("EAL Init failed"));
		logger->info("Done!");
		
		argc -= ret;
		argv += ret;
		
		if (int i = populate_aconf(&aconf, argc, argv) != 0) {
			return i;
		}
		
		sixelping::receiver::metrics::setup(&aconf);
		
		sixelping::receiver::frames::setup(&aconf);
		
		sixelping::receiver::ethdev::probe_ethernet_device(&aconf);
		
		unsigned nb_mbufs = RTE_MAX(
			aconf.ethdev.nb_rx_queues * (aconf.ethdev.rx_descs + aconf.ethdev.tx_descs + aconf.app.rx_burst_size) + rte_lcore_count() * MBUF_CACHE_SIZE,
			8192U);
		logger->info("Attempting to setup an mbuf pool with {} mbufs...", nb_mbufs);
		/* Creates a new mbuf mempool */
		aconf.mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", nb_mbufs, MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
		
		if (aconf.mbuf_pool == nullptr)
			throw_with_trace(sixelping::receiver::exceptions::dpdk_exception("mbuf_pool create failed"));
		
		sixelping::receiver::pixels::setup(&aconf);
		
		sixelping::receiver::ethdev::setup_ethernet_device(&aconf);
		
		sixelping::exit::setup();
		
		rte_eal_mp_remote_launch(sixelping::receiver::lcore::lcore_main, &aconf, CALL_MASTER);
		
		rte_eal_mp_wait_lcore();
		
		/* There is no un-init for eal */
		
		return 0;
	} catch (const std::exception& e) {
		logger->error("Caught exception: {}", sixelping::traced::print(e));
		exit(1);
	}
}
