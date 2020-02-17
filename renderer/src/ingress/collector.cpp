//
// Created by rewbycraft on 2/6/20.
//

#if 0
#define ENABLE_MUTEX
#include <mutex>
#endif
#include "sixelping/renderer/ingress/collector.h"
#include <sixelping/renderer/options.h>
#include <memory>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <prometheus/counter.h>

namespace sixelping::renderer::ingress::collector {
	using time_stamp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
	
	struct fadedpixel_t {
		uint8_t r, g, b;
		std::chrono::high_resolution_clock::time_point last_updated;
#ifdef ENABLE_MUTEX
		std::shared_ptr<std::mutex> mut;
#endif
	};
	
	std::vector<struct fadedpixel_t> image;
	std::shared_ptr<spdlog::logger> logger;
	
	void init() {
		logger = spdlog::stdout_color_mt("collector");
		logger->info("Initializing collector...");
		image.reserve(sixelping::renderer::options::get().width * sixelping::renderer::options::get().height);
		
		for (size_t i = 0; i < sixelping::renderer::options::get().width * sixelping::renderer::options::get().height; i++) {
			struct fadedpixel_t pix = {};
			pix.r = 0;
			pix.g = 0;
			pix.b = 0;
#ifdef ENABLE_MUTEX
			pix.mut = std::make_shared<std::mutex>();
#endif
			pix.last_updated = std::chrono::high_resolution_clock::now();
			image.push_back(pix);
		}
	}
	
	
	void handle_new_delta_image(const sixelping::Image<sixelping::rgba_pixel_t<uint16_t>> &delta) {
		unsigned w = delta.getWidth();
		unsigned h = delta.getHeight();
		for (unsigned y = 0; y < h; y++) {
			for (unsigned x = 0; x < w; x++) {
				unsigned i = y * w + x;
				struct fadedpixel_t &p = image[i];
				const auto& delta_pixel = delta.getData()[i];
				if (delta_pixel.a > 0) {
#ifdef ENABLE_MUTEX
					std::lock_guard<std::mutex> guard(*p.mut);
#endif
					p.r = delta_pixel.r / delta_pixel.a;
					p.g = delta_pixel.g / delta_pixel.a;
					p.b = delta_pixel.b / delta_pixel.a;
					p.last_updated = std::chrono::high_resolution_clock::now();
				}
			}
		}
	}
	
	void write_current_image(sixelping::Image<> &output_image, std::chrono::milliseconds fade_out, unsigned x_offset, unsigned y_offset, unsigned width, unsigned height) {
		auto now = std::chrono::high_resolution_clock::now();
		long f = fade_out.count();
		unsigned w = output_image.getWidth();
		unsigned h = output_image.getHeight();
		
		for (unsigned y = y_offset; y < std::min(y_offset + height, h); y++) {
			for (unsigned x = x_offset; x < std::min(x_offset +width, w); x++) {
				struct fadedpixel_t &original_pixel = image[y * w + x];
				auto& new_pixel = output_image.getPixel(x, y);
#ifdef ENABLE_MUTEX
				std::lock_guard<std::mutex> guard(*original_pixel.mut);
#endif
				auto then = original_pixel.last_updated;
				
				long diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - then).count();
				long m = f - diff;
				if (then >= now) {
					new_pixel.r = original_pixel.r;
					new_pixel.g = original_pixel.g;
					new_pixel.b = original_pixel.b;
				} else if ((diff >= f) || (original_pixel.r == 0 && original_pixel.g == 0 && original_pixel.b == 0)) {
					new_pixel.r = 0;
					new_pixel.g = 0;
					new_pixel.b = 0;
				} else {
					new_pixel.r = uint8_t(original_pixel.r * m / f);
					new_pixel.g = uint8_t(original_pixel.g * m / f);
					new_pixel.b = uint8_t(original_pixel.b * m / f);
				}
			}
		}
	}
}
