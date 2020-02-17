//
// Created by rewbycraft on 2/10/20.
//

#ifndef SIXELPING_PIXEL_H
#define SIXELPING_PIXEL_H

#include <cstdint>
#include <vector>

namespace sixelping {
	template<typename T = uint8_t>
	struct rgba_pixel_t;
	
	template<typename T = uint8_t>
	struct rgb_pixel_t {
		T r;
		T g;
		T b;
		
		inline rgb_pixel_t<T>& operator+=(const struct rgba_pixel_t<T>& rhs) {
			r = T( unsigned(r) * (255-unsigned(rhs.a)) / 255 + unsigned(rhs.r) * unsigned(rhs.a) / 255 );
			g = T( unsigned(g) * (255-unsigned(rhs.a)) / 255 + unsigned(rhs.g) * unsigned(rhs.a) / 255 );
			b = T( unsigned(b) * (255-unsigned(rhs.a)) / 255 + unsigned(rhs.b) * unsigned(rhs.a) / 255 );
			return *this;
		}
	} __attribute__((packed));
	
	template<typename T>
	struct rgba_pixel_t {
		T r;
		T g;
		T b;
		T a;
	} __attribute__((packed));
	
	template <typename pixel_type = struct rgb_pixel_t<>>
	class Image {
	public:
		using pixel_t = pixel_type;
		
		Image(unsigned int width, unsigned int height);
		
		[[nodiscard]] std::vector<pixel_t> &getData();
		[[nodiscard]] const std::vector<pixel_t> &getData() const;
		
		[[nodiscard]] unsigned int getWidth() const;
		
		[[nodiscard]] unsigned int getHeight() const;
		
		inline void setPixel(unsigned x, unsigned y, pixel_t pixel) {
			data[x+y*width] = pixel;
		}
		
		inline const pixel_t& getPixel(unsigned x, unsigned y) const {
			return data[x+y*width];
		}
		
		inline pixel_t& getPixel(unsigned x, unsigned y) {
			return data[x+y*width];
		}
		
		template <typename other_pixel_type>
		inline Image& operator+=(const Image<other_pixel_type>& rhs) {
			for (unsigned y = 0; y < height; y++) {
				for (unsigned x = 0; x < width; x++) {
					getPixel(x, y) += rhs.getPixel(x, y);
				}
			}
			
			return *this;
		}
	
	private:
		std::vector<pixel_t> data;
		unsigned width, height;
	};
}

#endif //SIXELPING_PIXEL_H
