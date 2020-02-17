//
// Created by rewbycraft on 2/10/20.
//

#include "sixelping/timed_loop.h"
#include <chrono>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <thread>

namespace sixelping {
	auto logger = spdlog::stdout_color_mt("timed_loop");
	
	void timed_loop(const std::string& name, double runs_per_second, const std::function<bool()> &fun) {
		using namespace std::chrono_literals;
		
		auto span = (1000ms / runs_per_second);
		std::chrono::time_point nextUpdate = std::chrono::steady_clock::now() + span;
		
		bool keepRunning = true;
		while (keepRunning) {
			std::this_thread::sleep_until(nextUpdate);
			
			std::chrono::time_point now = std::chrono::steady_clock::now();
			
			keepRunning = fun();
			
			std::chrono::time_point after = std::chrono::steady_clock::now();
			
			nextUpdate = now + span;
			
			if (after >= nextUpdate) {
				logger->warn("{} loop is running slow! A cycle took {} ms (instead of {} ms).", name, std::chrono::duration_cast<std::chrono::milliseconds>(after - now).count(), span.count());
			}
			
		}
	}
}
