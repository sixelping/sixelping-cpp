//
// Created by rewbycraft on 2/13/20.
//

#ifndef SIXELPING_CPP_EXCEPTIONS_H
#define SIXELPING_CPP_EXCEPTIONS_H

#include <exception>
#include <utility>
#include <sixelping/traced_error.h>
#include <rte_errno.h>

namespace sixelping::receiver::exceptions {
	class dpdk_exception : public std::exception {
		std::string nani;
	public:
		explicit dpdk_exception(const std::string& nani) {
			std::stringstream ss;
			ss << nani << " :: " << rte_strerror(rte_errno);
			this->nani = ss.str();
		}
		
		[[nodiscard]] const char *what() const noexcept override {
			return nani.c_str();
		}
	};
}

#endif //SIXELPING_CPP_EXCEPTIONS_H
