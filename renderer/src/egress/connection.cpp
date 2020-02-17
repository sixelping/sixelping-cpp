//
// Created by rewbycraft on 2/10/20.
//

#include <spdlog/sinks/stdout_color_sinks.h>
#include <sixelping/renderer/compositor/compositor.h>
#include "sixelping/renderer/egress/connection.h"
#include <sixelping/timed_loop.h>
#include <sixelping/renderer/options.h>

sixelping::renderer::egress::connection::connection(std::shared_ptr<boost::asio::ip::tcp::socket> socket) : socket(std::move(socket)) {
	std::stringstream ss;
	ss << "egress-" << this->socket->native_handle();
	logger = spdlog::stdout_color_mt(ss.str());
}

void sixelping::renderer::egress::connection::run() {
	boost::system::error_code error;
	const auto& opts = sixelping::renderer::options::get();
	
	logger->info("Starting egress of frames.");
	
	sixelping::timed_loop(logger->name(), opts.egress_fps, [&]() {
		auto img = sixelping::renderer::compositor::get_last_image();
		
		boost::asio::write(*socket, boost::asio::buffer(img->getData()), boost::asio::transfer_all(), error);
		
		return !error;
	});
	
	if (error) {
		logger->info("Transmission error: {}", error.message());
		return;
	}
	
}

sixelping::renderer::egress::connection::~connection() {
	spdlog::drop(logger->name());
}
