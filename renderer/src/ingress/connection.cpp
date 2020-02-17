//
// Created by rewbycraft on 2/6/20.
//

#include "sixelping/renderer/ingress/connection.h"
#include <sixelping/renderer/options.h>
#include <utility>
#include <sixelping/pixel.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <sixelping/renderer/ingress/collector.h>

namespace sixelping::renderer::ingress {
	
	connection::connection(std::shared_ptr<boost::asio::ip::tcp::socket> socket) : socket(std::move(socket)) {
		std::stringstream ss;
		ss << "ingress-" << this->socket->native_handle();
		logger = spdlog::stdout_color_mt(ss.str());
	}
	
	void connection::run() {
		boost::system::error_code error;
		
		sixelping::Image<sixelping::rgba_pixel_t<uint16_t>> img(sixelping::renderer::options::get().width, sixelping::renderer::options::get().height);
		
		while(!error) {
			
			size_t nb_recv = boost::asio::read(*socket, boost::asio::buffer(img.getData()), boost::asio::transfer_all(), error);
			
			if (img.getData().size() != img.getWidth()*img.getHeight()) {
				logger->error("AAAAAAAAAAAAAAAAAAA");
			}
			
			if (nb_recv != img.getData().size()*sizeof(sixelping::rgba_pixel_t<uint16_t>)) {
				logger->info("Incomplete read.");
				return;
			}
			
			std::chrono::time_point before = std::chrono::high_resolution_clock::now();
			sixelping::renderer::ingress::collector::handle_new_delta_image(img);
			std::chrono::time_point after = std::chrono::high_resolution_clock::now();
			
			long ms = std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count();
			if (ms > (1000 / sixelping::renderer::options::get().composit_fps)) {
				logger->warn("Ingress is slow: {} ms", ms);
			}
		}
		
		if (error) {
			logger->info("Receiver error: {}", error.message());
		}
	}
	
	connection::~connection() {
		spdlog::drop(logger->name());
	}
}