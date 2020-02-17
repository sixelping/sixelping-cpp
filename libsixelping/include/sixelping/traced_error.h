//
// Created by rewbycraft on 2/13/20.
//

#ifndef SIXELPING_CPP_TRACED_ERROR_H
#define SIXELPING_CPP_TRACED_ERROR_H

#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>

// https://www.boost.org/doc/libs/1_72_0/doc/html/stacktrace/getting_started.html#stacktrace.getting_started.exceptions_with_stacktrace
#define throw_with_trace(e) { \
		BOOST_THROW_EXCEPTION( boost::enable_error_info(e) \
			<< sixelping::traced::traced(boost::stacktrace::stacktrace())); \
	}

namespace sixelping::traced {
	typedef boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace> traced;

	
	template<typename E>
	std::string print(const E& e) {
		std::stringstream ss;
		ss << boost::diagnostic_information(e).c_str() << std::endl;
		const boost::stacktrace::stacktrace* st = boost::get_error_info<traced>(e);
		if (st) {
			ss << *st << std::endl;
		}
		return ss.str();
	}
}



#endif //SIXELPING_CPP_TRACED_ERROR_H
