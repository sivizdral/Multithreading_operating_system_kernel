#include "thread.h"
#include "pcb.h"
#include "timer.h"

void Thread::start()
{
	if (myPCB) myPCB->start();
}

void Thread::waitToComplete()
{
	if (myPCB) myPCB->waitToComplete();
}

Thread::~Thread()
{
	if (myPCB) {
		delete myPCB;
		myPCB = 0;
	}
}

ID Thread::getId()
{
	ID id = -1;
	if (myPCB) id = myPCB->id;
	return id;
}

ID Thread::getRunningId()
{
	return PCB::getRunningId();
}

Thread* Thread::getThreadById(ID id)
{
	return PCB::getThreadById(id);
}

Thread::Thread(StackSize stackSize, Time timeSlice)
{
	myPCB = new PCB(stackSize, timeSlice, this);
}

void dispatch()
{
	asm {
		pushf
		cli
	}
	Timer::requestedContextSwitch = 1;
	asm int 08h
	asm {
		popf
	}
}

ID Thread::fork() {
	ID id;
	if (PCB::running) {
		id = PCB::fork();
	} else {
		id = -1;
	}
	return id;
}

void Thread::exit() {
	PCB::exit();
}

void Thread::waitForForkChildren() {
	PCB::waitForForkChildren();
}

Thread* Thread::clone() const {
	return 0;
}
