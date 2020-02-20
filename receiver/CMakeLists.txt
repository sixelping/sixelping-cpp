set(RTE_SDK /usr/share/dpdk)
set(RTE_TARGET x86_64-default-linux-gcc)

add_executable(sixelping_receiver src/main.cpp include/sixelping/receiver/icmp6.h include/sixelping/receiver/common.h include/sixelping/receiver/application_configuration.h src/ethdev.cpp include/sixelping/receiver/ethdev.h src/packets.cpp include/sixelping/receiver/packets.h src/pixels.cpp include/sixelping/receiver/pixels.h src/lcoremain.cpp include/sixelping/receiver/lcoremain.h src/pktmbuf.c src/frames.cpp include/sixelping/receiver/frames.h src/metrics.cpp include/sixelping/receiver/metrics.h include/sixelping/receiver/exceptions.h)

#target_include_directories(sixelping_receiver PUBLIC ${RTE_SDK}/${RTE_TARGET}/include /usr/include/dpdk/ /usr/include/x86_64-linux-gnu/dpdk/)
#target_link_directories(sixelping_receiver PUBLIC ${RTE_SDK}/${RTE_TARGET}/lib)

#SET(DPDK_LIBS "-Wl,--whole-archive -ldpdk -Wl,--no-whole-archive")
#SET(DPDK_LIBS "-Wl,--whole-archive  -lrte_bpf -lrte_flow_classify -lrte_pipeline -lrte_table -lrte_port -lrte_fib -lrte_ipsec -lrte_vhost -lrte_stack -lrte_security -lrte_sched -lrte_reorder -lrte_rib -lrte_rcu -lrte_rawdev -lrte_pdump -lrte_power -lrte_member -lrte_lpm -lrte_latencystats -lrte_kni -lrte_jobstats -lrte_ip_frag -lrte_gso -lrte_gro -lrte_eventdev -lrte_efd -lrte_distributor -lrte_cryptodev -lrte_compressdev -lrte_cfgfile -lrte_bitratestats -lrte_bbdev -lrte_acl -lrte_timer -lrte_hash -lrte_metrics -lrte_cmdline -lrte_pci -lrte_ethdev -lrte_meter -lrte_net -lrte_mbuf -lrte_mempool -lrte_ring -lrte_eal -lrte_kvargs -lbsd -Wl,--no-whole-archive")
#target_link_libraries(sixelping_receiver PUBLIC ${DPDK_LIBS})

target_link_libraries(sixelping_receiver PUBLIC dpdk::dpdk)
target_include_directories(sixelping_receiver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(sixelping_receiver PUBLIC pthread dl rt m numa)
target_link_libraries(sixelping_receiver PUBLIC libsixelping)
target_link_libraries(sixelping_receiver PUBLIC prometheus-cpp::pull)
set_target_properties(sixelping_receiver PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS})
target_link_libraries(sixelping_receiver PUBLIC Boost::program_options Threads::Threads spdlog::spdlog Boost::thread)

package_add_benchmark(pixelbenchmark tests/pixelbenchmark.cpp)
