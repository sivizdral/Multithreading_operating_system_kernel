#ifndef _kernsem_h_
#define _kernsem_h_

#include "semaphor.h"

class SemList;
class Queue;

class KernelSem {
public:
	friend class Semaphore;
	friend class semList;
	KernelSem(Semaphore* sem, int init);
	~KernelSem();

	int wait(Time maxTimeToWait);
	void signal();

	static void decrementAllSemaphores();
	void decrementThisSemaphore();

	int value;
	Semaphore* thisSemaphore;
	Queue* waitingQueue;
	static SemList* semaphoreList;
};

#endif
