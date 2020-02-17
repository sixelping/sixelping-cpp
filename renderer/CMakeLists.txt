add_executable(sixelping_renderer src/main.cpp src/ingress/connection.cpp include/sixelping/renderer/ingress/connection.h src/ingress/collector.cpp include/sixelping/renderer/ingress/collector.h src/options.cpp include/sixelping/renderer/options.h src/compositor/compositor.cpp include/sixelping/renderer/compositor/compositor.h src/egress/connection.cpp include/sixelping/renderer/egress/connection.h src/compositor/overlay.cpp include/sixelping/renderer/compositor/overlay.h src/prometheus.cpp include/sixelping/renderer/prometheus.h src/metrics.cpp include/sixelping/renderer/metrics.h)

target_include_directories(sixelping_renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(sixelping_renderer PUBLIC Boost::program_options Threads::Threads spdlog::spdlog Boost::thread)
target_include_directories(sixelping_renderer PUBLIC "${ImageMagick_INCLUDE_DIRS}")
target_link_libraries(sixelping_renderer PUBLIC ${ImageMagick_LIBRARIES})
target_link_libraries(sixelping_renderer PUBLIC libsixelping)
target_link_libraries(sixelping_renderer PUBLIC cpr)
target_include_directories(sixelping_renderer PUBLIC ${RAPIDJSON_INCLUDE_DIRS})
