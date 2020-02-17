//
// Created by rewbycraft on 2/13/20.
//

#include "sixelping/exitcontrol.h"
#include <atomic>
#include <csignal>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace sixelping::exit {
	std::atomic_bool should_exit_flag;
	std::shared_ptr<spdlog::logger> logger;
	int exit_code = 0;
	
	bool should_exit() {
		return should_exit_flag.load();
	}
	
	static void signal_handler(int signum) {
		if (signum == SIGINT || signum == SIGTERM) {
			logger->info("Signal {} received. Exiting...", signum);
			make_exit();
		}
	}
	
	void setup() {
		logger = spdlog::stdout_color_mt("exit");
		should_exit_flag.store(false);
		
		struct sigaction act = {};
		memset(&act, 0, sizeof(struct sigaction));
		act.sa_flags = SA_RESETHAND;
		act.sa_handler = signal_handler;
		sigaction(SIGINT, &act, NULL);
		sigaction(SIGTERM, &act, NULL);
		
		logger->info("Registered signal handler!");
	}
	
	void make_exit(int rc) {
		if (!should_exit()) {
			if (rc != 0) {
				exit_code = rc;
			}
			logger->info("Exit requested!");
			should_exit_flag.store(true);
		}
	}
}
