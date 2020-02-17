//
// Created by rewbycraft on 2/6/20.
//

#ifndef SIXELPING_TCPSERVER_H
#define SIXELPING_TCPSERVER_H

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <boost/thread.hpp>
#include <exception>

namespace sixelping {
	template<class tcp_connection>
	class tcp_server {
	public:
		tcp_server(boost::asio::io_context &io_context, unsigned port) : io_context_(io_context),
		                                                                 acceptor_(io_context,
		                                                                           boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), port)) {
			std::stringstream ss;
			ss << "tcp_server-" << this->acceptor_.native_handle();
			logger = spdlog::stdout_color_mt(ss.str());
			
			logger->info("Listening on [{}]:{}.", acceptor_.local_endpoint().address().to_string(), acceptor_.local_endpoint().port());
			start_accept();
		}
		
		~tcp_server() {
			spdlog::drop(logger->name());
		}
	
	private:
		void start_accept() {
			auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context_);
			
			acceptor_.async_accept(*socket, boost::bind(&tcp_server::handle_accept, this, socket, boost::asio::placeholders::error));
		}
		
		void handle_accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code &error) {
			if (!error) {
				logger->info("Accepted connection to [{}]:{}.", acceptor_.local_endpoint().address().to_string(), acceptor_.local_endpoint().port());
				
				//Launch socket thread.
				boost::thread t(boost::bind(&tcp_server::socket_thread, this, socket));
			}
			
			start_accept();
		}
		
		void socket_thread(std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
			try {
				tcp_connection conn(socket);
				conn.run();
				logger->info("TCP connection handler exited.");
			} catch (const std::exception &e) {
				logger->error("Caught exception from tcp connection: {}", e.what());
			}
			
			if (socket->is_open()) {
				try {
					socket->shutdown(boost::asio::socket_base::shutdown_both);
				} catch (std::exception &e) {
					//Ignore shutdown failures.
				}
				socket->close();
			}
		}
		
		boost::asio::io_context &io_context_;
		boost::asio::ip::tcp::acceptor acceptor_;
		std::shared_ptr<spdlog::logger> logger;
	};
	
}

#endif //SIXELPING_TCPSERVER_H
