//
// Created by rewbycraft on 2/12/20.
//

#include <sixelping/timed_loop.h>
#include <sixelping/receiver/application_configuration.h>
#include <sixelping/receiver/pixels.h>
#include "sixelping/receiver/frames.h"
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <sixelping/exitcontrol.h>
#include <sixelping/traced_error.h>
#include <regex>

namespace sixelping::receiver::frames {
	
	void setup(struct app_config *aconf) {
	
	}
	
	void run(struct app_config *aconf) {
		auto logger = spdlog::stdout_color_mt("frames");
		boost::asio::io_service io_service;
		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::system::error_code error;
		logger->info("Attempting to connect to {}...", aconf->frames.endpoint);
		
		std::regex r("^(.*)((?::))((?:[0-9]+))$");
		std::smatch m;
		std::regex_search(aconf->frames.endpoint, m, r);
		
		boost::asio::ip::tcp::resolver::query query(m[1], "");
		auto endpoint_iterator = resolver.resolve(query);
		boost::asio::ip::tcp::resolver::iterator end;
		
		boost::asio::ip::tcp::socket socket(io_service);
		error = boost::asio::error::host_not_found;
		while (error && endpoint_iterator != end) {
			socket.close();
			boost::asio::ip::tcp::endpoint ep = *endpoint_iterator++;
			ep.port(std::stoi(m[3]));
			socket.connect(ep, error);
		}
		
		if (error) {
			logger->error("Failed to connect to host: {}", error.message());
			return;
		}
		
		sixelping::timed_loop("frames", aconf->frames.fps, [&]() {
			auto new_frame = sixelping::receiver::pixels::swap_buffers(aconf);
			
			boost::asio::write(socket, boost::asio::buffer(new_frame->getData()), error);
			
			return ((!error) && (!sixelping::exit::should_exit()));
		});
		
		logger->info("Socket exited: {}", error.message());
		
	}
}
