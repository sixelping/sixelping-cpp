//
// Created by rewbycraft on 2/6/20.
//

#ifndef SIXELPING_RENDERER_CONNECTION_H
#define SIXELPING_RENDERER_CONNECTION_H

#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

namespace sixelping::renderer::ingress {
	class connection {
	public:
		explicit connection(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
		
		void run();
	
		~connection();
	private:
		std::shared_ptr<spdlog::logger> logger;
		std::shared_ptr<boost::asio::ip::tcp::socket> socket;
	};
}


#endif //SIXELPING_RENDERER_CONNECTION_H
