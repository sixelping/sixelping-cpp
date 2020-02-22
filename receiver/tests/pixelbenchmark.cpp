#include <benchmark/benchmark.h>
#include <sixelping/receiver/common.h>
#include <sixelping/receiver/icmp6.h>
#include <sixelping/receiver/exceptions.h>
#include <iostream>
#include <sixelping/receiver/application_configuration.h>
#include <sixelping/receiver/pixels.h>
#include <sixelping/receiver/packets.h>

struct rte_mempool *mbuf_pool;
struct app_config aconf = {};

class MyFixture : public benchmark::Fixture {
public:
	void SetUp(const benchmark::State &state) override {
	
	}
	
	void TearDown(const benchmark::State &state) override {
	}
};

struct rte_mbuf *make_ping_packet(struct rte_mempool *mbuf_pool) {
	struct rte_mbuf *newpkt = rte_pktmbuf_alloc(mbuf_pool);
	auto *neweth = (struct rte_ether_hdr *) rte_pktmbuf_append(newpkt, sizeof(struct rte_ether_hdr));
	auto *newip = (struct rte_ipv6_hdr *) rte_pktmbuf_append(newpkt, sizeof(struct rte_ipv6_hdr));
	auto *newicmp = (struct icmp6_hdr *) rte_pktmbuf_append(newpkt, sizeof(struct icmp6_hdr));
	
	//Null everything
	memset(neweth, 0, sizeof(struct rte_ether_hdr));
	memset(newip, 0, sizeof(struct rte_ipv6_hdr));
	memset(newicmp, 0, sizeof(struct icmp6_hdr));
	
	//Setup mac addresses in L2 header
	neweth->ether_type = htons(RTE_ETHER_TYPE_IPV6);
	
	//Setup ip addresses and proto in L3 header
	newip->proto = ICMP6_PROTO;
	newip->hop_limits = 0xff;
	newip->vtc_flow = htonl(0x60000000); //IPv6 header magic
	//Compute length of payload
	newip->payload_len = htons(sizeof(struct icmp6_hdr) + sizeof(struct ndp_sollicit_advertise_hdr) + sizeof(struct ndp_tll_opt_hdr));
	
	//Setup ICMP header
	newicmp->type = 128; //NDP Advertisement
	
	//Compute icmp6 checksum.
	//This reimplements rte_ipv6_udptcp_cksum to allow multiple headers to be processed easily.
	struct temp_t {
		struct icmp6_hdr icmp;
	} __attribute__((__packed__));
	
	struct temp_t *entireicmp = static_cast<temp_t *>(rte_malloc("ICMP6PKT", sizeof(struct temp_t), 0));
	memcpy(&entireicmp->icmp, newicmp, sizeof(struct icmp6_hdr));
	newicmp->checksum = rte_ipv6_udptcp_cksum(newip, entireicmp);
	rte_free(entireicmp);
	
	return newpkt;
	
}

BENCHMARK_F(MyFixture, ping_bench)(benchmark::State &state) {
	struct rte_mbuf *pkt = make_ping_packet(mbuf_pool);
	
	for (auto _ : state) {
		sixelping::receiver::packets::process_packet(&aconf, pkt);
		
	}
	
	nipktmfree(pkt);
}


int main(int argc, char **argv) {
	if (rte_eal_init(argc, argv)) {
		throw_with_trace(sixelping::receiver::exceptions::dpdk_exception("EAL Init failed"));
	}
	mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", 8192, MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
	if (mbuf_pool == NULL) {
		throw_with_trace(sixelping::receiver::exceptions::dpdk_exception("mbuf_pool create failed"));
	}
	
	aconf.pixels.width = 1920;
	aconf.pixels.height = 1080;
	sixelping::receiver::pixels::setup(&aconf);
	
	::benchmark::Initialize(&argc, argv);
	if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
	::benchmark::RunSpecifiedBenchmarks();
	
	rte_eal_cleanup();
}

