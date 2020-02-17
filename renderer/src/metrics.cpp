//
// Created by rewbycraft on 2/17/20.
//

#include "sixelping/renderer/metrics.h"
#include <prometheus/exposer.h>
#include <boost/format.hpp>
#include <sixelping/renderer/options.h>

namespace sixelping::renderer::metrics {
	std::shared_ptr<prometheus::Registry> registry;
	std::shared_ptr<prometheus::Exposer> exposer;
	
	void init() {
		const auto& opts = sixelping::renderer::options::get();
		registry = std::make_shared<prometheus::Registry>();
		exposer = std::make_shared<prometheus::Exposer>(boost::str(boost::format("[::]:%d") % opts.metrics_port));
		exposer->RegisterCollectable(registry);
	}
	
	prometheus::Registry &get_registry() {
		return *registry;
	}
}
