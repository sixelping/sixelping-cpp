add_library(libsixelping src/pixel.cpp src/timed_loop.cpp include/sixelping/pixel.h include/sixelping/timed_loop.h include/sixelping/tcpserver.h src/exitcontrol.cpp include/sixelping/exitcontrol.h include/sixelping/traced_error.h include/sixelping/logs.h src/logs.cpp src/promutil.cpp include/sixelping/promutil.h)
target_include_directories(libsixelping PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(libsixelping PUBLIC Threads::Threads spdlog::spdlog Boost::stacktrace_basic)
target_link_libraries(libsixelping PUBLIC prometheus-cpp::pull)
