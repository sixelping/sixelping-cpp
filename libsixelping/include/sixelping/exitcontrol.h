//
// Created by rewbycraft on 2/13/20.
//

#ifndef SIXELPING_CPP_EXITCONTROL_H
#define SIXELPING_CPP_EXITCONTROL_H

namespace sixelping::exit {
	bool should_exit();
	void setup();
	void make_exit(int rc = 0);
}

#endif //SIXELPING_CPP_EXITCONTROL_H
