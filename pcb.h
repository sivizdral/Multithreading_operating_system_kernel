#ifndef _pcb_h_
#define _pcb_h_

#include "SCHEDULE.H"
#include "thread.h"

class Idle;
class Thread;

#define lock {Timer::lockCount++;}
#define unlock {Timer::lockCount--;\
	if (Timer::lockCount == 0) {\
		if (Timer::counter == 0 && PCB::running->timeSlice != 0)\
		dispatch();\
	}\
}

class pcbList;

const StackSize maximumStackSize = 65535;


class PCB {
public:

	// list of all states a thread can be in (obrisati running, idle?)
	typedef enum StateOfThread { CREATED, READY, BLOCKED, FINISHED, IDLE } StateOfThread;

	// context
	unsigned sp;
	unsigned ss;
	unsigned bp;

	// stack
	unsigned* stack;
	StackSize stackSize;

	// state
	StateOfThread state;

	// ID
	ID id;
	static ID identification;

	// time: how long does the thread's execution last
	unsigned timeSlice;

	// this thread
	Thread* thisThread;

	// list of all threads that are waiting for this thread to finish
	pcbList* waitingToCompletePCBList;

	// check if signal unblocked this thread (to return proper value in KernelSem::wait)
	int signalUnblocking;

	// running thread (nije volatile)
	static PCB* running;

	// idle thread
	static PCB* idle;

	// global list of all PCBs
	static pcbList* globalPCBList;

	// list of this thread's children
	pcbList* myChildren;

	// this thread's parent
	PCB* myParent;

	// is this thread waiting for its children
	int waitingForChildren;

	PCB(StackSize stackSize, Time timeSlice, Thread* thisThread);
	void start();
	void waitToComplete();
	static void wrapper();
	void clearWaitingToCompletePCBList();
	void tellMyParentIFinished();
	void checkIfParentCanRetire();
	void tellChildrenIRetired();
	static Thread* getThreadById(ID id);
	static ID getRunningId();
	static void interrupt makeChildsStack(...);
	static ID fork();
	static void exit();
	static void waitForForkChildren();
	~PCB();
};

#endif
