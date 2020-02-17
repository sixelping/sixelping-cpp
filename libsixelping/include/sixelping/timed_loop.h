//
// Created by rewbycraft on 2/10/20.
//

#ifndef SIXELPING_TIMED_LOOP_H
#define SIXELPING_TIMED_LOOP_H
#include <functional>

namespace sixelping {
	void timed_loop(const std::string& name, double runs_per_second, const std::function<bool()>& fun);
}

#endif //SIXELPING_COMPOSITOR_TIMED_LOOP_H
