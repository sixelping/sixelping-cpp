#include <boost/asio.hpp>
#include <sixelping/tcpserver.h>
#include <sixelping/renderer/ingress/connection.h>
#include <sixelping/renderer/ingress/collector.h>
#include <sixelping/renderer/egress/connection.h>
#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <sixelping/renderer/options.h>
#include <sixelping/renderer/compositor/compositor.h>
#include <sixelping/renderer/compositor/overlay.h>
#include <sixelping/renderer/metrics.h>

int main(int argc, char* argv[])
{
	if (int i = sixelping::renderer::options::parse(argc, argv)) {
		return i;
	}
	
	auto logger = spdlog::stdout_color_mt("main");
	try
	{
		logger->info("Sixelping Compositor");
		
		sixelping::renderer::metrics::init();
		sixelping::renderer::ingress::collector::init();
		sixelping::renderer::compositor::overlay::init();
		sixelping::renderer::compositor::init();
		
		const auto& opts = sixelping::renderer::options::get();
		
		boost::asio::io_context io_context;
		
		sixelping::tcp_server<sixelping::renderer::ingress::connection> server(io_context, opts.ingress_port);
		sixelping::tcp_server<sixelping::renderer::egress::connection> server2(io_context, opts.egress_port);

		auto thread = sixelping::renderer::compositor::start();
		auto thread2 = sixelping::renderer::compositor::overlay::start();
		
		io_context.run();
		
		logger->error("This should never happen!");
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
	return 0;
}
