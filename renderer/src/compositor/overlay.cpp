//
// Created by rewbycraft on 2/10/20.
//

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include "sixelping/renderer/compositor/overlay.h"
#include <boost/smart_ptr/atomic_shared_ptr.hpp>
#include <sixelping/timed_loop.h>
#include <sixelping/renderer/options.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <sstream>
#include <sixelping/renderer/compositor/compositor.h>
#include <sixelping/renderer/prometheus.h>
#include <boost/format.hpp>
#include <sixelping/renderer/metrics.h>
#include <prometheus/counter.h>
#include <sixelping/exitcontrol.h>
#include <Magick++.h>

namespace sixelping::renderer::compositor::overlay {
	std::shared_ptr<spdlog::logger> logger;
	boost::atomic_shared_ptr<Magick::Image> last_image;
	Magick::Image logo_image;
	
	boost::shared_ptr<Magick::Image> create_overlay_image() {
		const auto &opts = sixelping::renderer::options::get();
		auto img = boost::make_shared<Magick::Image>(Magick::Geometry(opts.width, opts.height), Magick::Color("transparent"));
		img->strokeColor(Magick::Color("transparent"));
		img->fillColor(Magick::Color("white"));
		img->strokeWidth(1);
		img->fontPointsize(opts.font_size);
		img->font(opts.font);
		
		std::stringstream ss;
		auto v = sixelping::renderer::prometheus::query("sum(receiver_packets_received_per_second)");
		if (v.has_value()) {
			if (!ss.str().empty())
				ss << " | ";
			ss << (boost::format("%.02f Mpps") % (v.value() / 1000000.0));
		}
		
		v = sixelping::renderer::prometheus::query("sum(receiver_bits_received_per_second)");
		if (v.has_value()) {
			if (!ss.str().empty())
				ss << " | ";
			ss << (boost::format("%.02f Gbps") % (v.value() / 1000000000.0));
		}
		
		v = sixelping::renderer::prometheus::query("sum(receiver_packets_dropped_per_second)");
		if (v.has_value() && v.value() > 0) {
			if (!ss.str().empty())
				ss << " | ";
			ss << "Sixelping is currently overloaded and is dropping some of your pings.";
			img->fillColor(Magick::Color("red"));
		}
		
		if (!opts.logo.empty())
			img->composite(logo_image, 10, img->rows() - 10 - logo_image.rows(), Magick::OverlayCompositeOp);
		
		img->draw(Magick::DrawableText(10 + logo_image.columns() + 10, img->rows() - 10, ss.str()));
		return img;
	}
	
	void init() {
		const auto &opts = sixelping::renderer::options::get();
		logger = spdlog::stdout_color_mt("overlay");
		logger->info("Initializing overlayer...");
		
		if (!opts.logo.empty())
			logo_image.read(opts.logo);
		
		logger->info("Writing initial frame...");
		last_image.store(create_overlay_image());
	}
	
	
	void write_current_image(sixelping::Image<> &output_image, unsigned x_offset, unsigned y_offset, unsigned width, unsigned height) {
		Magick::Pixels view(*(last_image.load()));
		
		for (unsigned int j = y_offset; j < std::min(y_offset + height, output_image.getHeight()); j++) {
			
			const Magick::Quantum *pixels = view.getConst(0, j, output_image.getWidth(), 1);
			
			for (unsigned int i = x_offset; i < std::min(x_offset + width, output_image.getWidth()); i++) {
				unsigned index = 4 * i;
				
				uint8_t r = 255 * QuantumScale * pixels[index + 0];
				uint8_t g = 255 * QuantumScale * pixels[index + 1];
				uint8_t b = 255 * QuantumScale * pixels[index + 2];
				uint8_t a = 255 * QuantumScale * pixels[index + 3];
				
				if (a > 0) {
					auto &pix = output_image.getPixel(i, j);
					pix.r = (r * a / 255) + (pix.r * (255 - a) / 255);
					pix.g = (g * a / 255) + (pix.g * (255 - a) / 255);
					pix.b = (b * a / 255) + (pix.b * (255 - a) / 255);
				}
			}
		}
	}
	
	void run() {
		auto& frame_counter = ::prometheus::BuildCounter().Name("renderer_overlayed_frames_total").Register(sixelping::renderer::metrics::get_registry()).Add({});
		
		try {
			logger->info("Entering overlaying loop...");
			const auto &opts = sixelping::renderer::options::get();
			
			sixelping::timed_loop("overlay", opts.overlay_fps, [&]() {
				auto img = create_overlay_image();
				
				last_image.store(img);
				
				frame_counter.Increment();
				
				return !sixelping::exit::should_exit();
			});
		} catch (std::exception &e) {
			logger->error("Exception in overlay loop: {}", e.what());
			throw e;
		}
	}
	
	std::thread start() {
		return std::thread(run);
	}
}
