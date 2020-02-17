//
// Created by rewbycraft on 2/10/20.
//

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include "sixelping/renderer/compositor/compositor.h"
#include <boost/smart_ptr/atomic_shared_ptr.hpp>
#include <sixelping/timed_loop.h>
#include <sixelping/renderer/options.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <sixelping/renderer/ingress/collector.h>
#include <chrono>
#include <sixelping/renderer/compositor/overlay.h>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <future>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <prometheus/counter.h>
#include <sixelping/renderer/metrics.h>
#include <sixelping/exitcontrol.h>

namespace sixelping::renderer::compositor {
	std::shared_ptr<spdlog::logger> logger;
	boost::atomic_shared_ptr<sixelping::Image<>> last_image;
	std::chrono::milliseconds last_loop_time;
	
	void init() {
		logger = spdlog::stdout_color_mt("renderer");
		logger->info("Initializing renderer...");
		
		//Create and store empty image
		const auto &opts = sixelping::renderer::options::get();
		last_image.store(boost::make_shared<sixelping::Image<>>(opts.width, opts.height));
	}
	
	boost::shared_ptr<sixelping::Image<>> get_last_image() {
		return last_image.load();
	}
	
	std::chrono::milliseconds get_last_loop_time() {
		return last_loop_time;
	}
	
	void run() {
		try {
			logger->info("Entering composition loop...");
			auto& frame_counter = prometheus::BuildCounter().Name("renderer_composited_frames_total").Register(sixelping::renderer::metrics::get_registry()).Add({});
			const auto &opts = sixelping::renderer::options::get();
			using namespace std::chrono_literals;
			auto fade_out_duration = std::chrono::duration_cast<std::chrono::milliseconds>(1000ms * opts.fade_out);
			
			boost::asio::thread_pool pool(16);
			boost::barrier barrier(16+1);
			
			sixelping::timed_loop("composit", opts.composit_fps, [&]() {
				auto before = std::chrono::high_resolution_clock::now();
				
				boost::shared_ptr<sixelping::Image<>> img = boost::make_shared<sixelping::Image<>>(opts.width, opts.height);
				
				for (unsigned x = 0; x < opts.width; x += opts.width / 4) {
					for (unsigned y = 0; y < opts.height; y += opts.height / 4) {
						std::function<void()> fun = [&barrier, opts, img, fade_out_duration, x, y]() {
							sixelping::renderer::ingress::collector::write_current_image(*img, fade_out_duration, x, y, opts.width/4, opts.height/4);
							sixelping::renderer::compositor::overlay::write_current_image(*img, x, y, opts.width/4, opts.height/4);
							
							//Notify that we're done.
							barrier.wait();
						};
						
						boost::asio::post(pool, fun);
					}
				}
				
				//Wait for the threads to complete.
				barrier.wait();
				
				last_image.store(img);
				
				auto after = std::chrono::high_resolution_clock::now();
				last_loop_time = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
				
				frame_counter.Increment();
				
				return !sixelping::exit::should_exit();
			});
		} catch (std::exception &e) {
			logger->error("Exception in composition loop: {}", e.what());
			throw e;
		}
	}
	
	std::thread start() {
		return std::thread(run);
	}
}
