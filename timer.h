#ifndef _timer_h_
#define _timer_h_

#include "pcb.h"
#include <dos.h>
#include "kernSem.h"

typedef void interrupt (*intPointer)(...);
int syncPrintf(const char *format, ...);

class Timer {
public:

	static void interrupt timer(...);
	static void initialize();
	static void restore();

	static intPointer old;
	static int requestedContextSwitch;
	static int counter;
	static int lockCount;

};

#endif
