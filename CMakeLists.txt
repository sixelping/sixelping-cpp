cmake_minimum_required(VERSION 3.11)
project(sixelping_cpp)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wno-unused-parameter -ggdb -g -march=native")
set(CMAKE_CXX_STANDARD 17)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.67 REQUIRED COMPONENTS program_options thread stacktrace_basic)
find_package(spdlog REQUIRED)
find_package(Threads REQUIRED)
find_package(ImageMagick REQUIRED COMPONENTS Magick++)
find_package(RapidJSON REQUIRED)
find_package(dpdk REQUIRED)
message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "spdlog version: ${spdlog_VERSION}")
message(STATUS "Magick++ version: ${ImageMagick_VERSION_STRING}")
message(STATUS "Magick++ Include dirs: ${ImageMagick_INCLUDE_DIRS}")
message(STATUS "Magick++ Libraries: ${ImageMagick_LIBRARIES}")

add_definitions(-DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_QUANTUM_DEPTH=16)

include(FetchContent)

# https://cliutils.gitlab.io/modern-cmake/chapters/projects/fetch.html
if(${CMAKE_VERSION} VERSION_LESS 3.14)
macro(FetchContent_MakeAvailable NAME)
	FetchContent_GetProperties(${NAME})
	if(NOT ${NAME}_POPULATED)
		FetchContent_Populate(${NAME})
		add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
	endif()
endmacro()
endif()

message(STATUS "Prometheus-CPP FetchContent")
FetchContent_Declare(
	prometheus-cpp
	GIT_REPOSITORY https://github.com/jupp0r/prometheus-cpp.git
	GIT_TAG        v0.9.0
)

FetchContent_MakeAvailable(prometheus-cpp)

message(STATUS "Subdirs")

add_subdirectory(vendor/cpr)
add_subdirectory(libsixelping)
add_subdirectory(renderer)
add_subdirectory(receiver)