#ifndef _idle_h_
#define _idle_h_

#include "thread.h"

// when all other threads are blocked/waiting, or there aren't any other threads
// ready to go to the scheduler at the moment, this idle thread is the one that
// is called to work in the timer interrupt routine; it is basically doing nothing

class Idle : public Thread {
public:
	Idle(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice) : Thread(stackSize, timeSlice) {};
	void run() { while(1); }
};

#endif
