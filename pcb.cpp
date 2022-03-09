#include "pcb.h"
#include <dos.h>
#include "pcbList.h"
#include "timer.h"
#include "idle.h"
#include "thread.h"
#include <dos.h>
#include <string.h>

ID PCB::identification = 0;
pcbList* PCB::globalPCBList = new pcbList();
PCB* PCB::running = new PCB(4096,2,0); // main thread
PCB* PCB::idle = (new Idle(1024,1))->myPCB; // idle thread
PCB* globalParent; // global variables used for fork
PCB* globalChild;
unsigned int ttsp, ttbp, ttss;
unsigned* oldSP, *newSP, *oldBP, *newBP, *nextBP, *oldnx;

PCB::PCB(StackSize stackSize, Time timeSlice, Thread* thisThread)
{
	lock
	// stack creation
	StackSize size = 0;
	if (stackSize > maximumStackSize) size = maximumStackSize / sizeof(unsigned);
	else if (stackSize == 1) size = 1024 / sizeof(unsigned);
	else size = stackSize / sizeof(unsigned);
	this->stack = new unsigned[size];
	if (this->stack == 0) return;
	this->stackSize = size;

	// stack initialization
#ifndef BCC_BLOCK_IGNORE
	this->stack[size - 1] = 0x200;  // program status word (PSW) is 0x200
	this->stack[size - 2] = FP_SEG(&PCB::wrapper);
	this->stack[size - 3] = FP_OFF(&PCB::wrapper);
	this->stack[size - 12] = 0;
	ss = FP_SEG(this->stack + size - 12);
	sp = FP_OFF(this->stack + size - 12);
	bp = sp;

#endif

	// time and state initialization etc.

	this->timeSlice = timeSlice;
	this->thisThread = thisThread;
	this->id = identification++;
	this->signalUnblocking = 0;
	this->myParent = 0;
	this->waitingForChildren = 0;

	if (id == 0) this->state = READY;
	else this->state = CREATED;

	// add to a global list of all threads
	((pcbList*)globalPCBList)->addPCB(this);

	// initialize waitingToComplete list and children list
	waitingToCompletePCBList = new pcbList();
	myChildren = new pcbList();
	unlock
}

void PCB::start() {
	lock
	if (state == CREATED) {
		state = READY;
		Scheduler::put(this);
	}
	unlock
}

void PCB::waitToComplete() {
	lock
	if (this != 0 && this != (PCB*)PCB::running && this->state != FINISHED && this->state != CREATED && this->state != IDLE) {
		PCB::running->state = BLOCKED;
		waitingToCompletePCBList->addPCB((PCB*)PCB::running);
		unlock
		dispatch();
	}
	else unlock;
}


// calls run method, and changes state to finished afterwards; clears the list of all the threads
// that waited for this thread to complete;
void PCB::wrapper()
{
	PCB::running->thisThread->run();
	lock
	((PCB*)PCB::running)->clearWaitingToCompletePCBList();
	PCB::running->state = FINISHED;
	PCB::running->tellMyParentIFinished();
	PCB::running->tellChildrenIRetired();
	unlock
	dispatch();
}

// empties the list of all waiting threads, changes their statuses to ready and puts them into scheduler
void PCB::clearWaitingToCompletePCBList()
{
	lock
	PCB* current;
	for (waitingToCompletePCBList->setCurrentToFirst();
		waitingToCompletePCBList->hasCurrent(); waitingToCompletePCBList->next()) {
		current = waitingToCompletePCBList->getCurrent();
		current->state = READY;
		Scheduler::put(current);
	}
	waitingToCompletePCBList->clear();
	unlock
}

// checks if there exists a pcb with given ID in the global list, and then returns its thread if it does
// or 0 if it doesn't
Thread* PCB::getThreadById(ID id)
{
	lock
	PCB* pcb = ((pcbList*)globalPCBList)->findByID(id);
	Thread* thread = 0;
	if (pcb != 0) {
		thread = pcb->thisThread;
		unlock
		return thread;
	}
	else {
		unlock
		return thread;
	}
}

ID PCB::getRunningId() {
	return PCB::running->id;
}

PCB::~PCB() {
	lock
	((pcbList*)globalPCBList)->remove(this);
	if (stack != 0) delete[] stack;
	clearWaitingToCompletePCBList();
	tellMyParentIFinished();
	tellChildrenIRetired();
	unlock
}

void interrupt PCB::makeChildsStack(...) {
	asm {
		pushf
		cli
	}
	memcpy(globalChild->stack, globalParent->stack, globalParent->stackSize * sizeof(unsigned));
	asm {
		popf
	}
#ifndef BCC_BLOCK_IGNORE
		asm {
			mov ttss, ss
			mov ttbp, bp
			mov ttsp, sp
		}
		globalChild->bp = 0;
		globalChild->sp = 0;
		globalChild->ss = 0;
		oldSP = 0;
		newSP = 0;
		newBP = 0;
		oldBP = 0;
		oldSP = (unsigned*) MK_FP(ttss, ttsp);
		newSP = oldSP - globalParent->stack + globalChild->stack;
		oldBP = (unsigned*) MK_FP(ttss, ttbp);
		newBP = oldBP - globalParent->stack + globalChild->stack;
		globalChild->bp = FP_OFF(newBP);
		globalChild->sp = FP_OFF(newSP);
		globalChild->ss = FP_SEG(newSP);
		globalChild->stackSize = globalParent->stackSize;
		nextBP = 0;
		oldnx = 0;
		for (; (*newBP) != 0; oldBP = oldnx, newBP = nextBP) {
			oldnx = (unsigned*) MK_FP(ttss, (*newBP));
			nextBP = oldnx - oldBP + newBP;
			*newBP = FP_OFF(nextBP);
		}
		globalChild->start();
#endif
}

ID PCB::fork() {
	lock
	ID childID = -1;
	Thread* newChild = PCB::running->thisThread->clone();
	if (!newChild){
		unlock
		return childID;
	}
	if (!newChild->myPCB->stack) {
		unlock
		return childID;
	}
	Thread* currParent = PCB::running->thisThread;
	if (currParent->myPCB->stackSize > newChild->myPCB->stackSize) {
		unlock
		return childID;
	}
	else {
		PCB* child = newChild->myPCB;
		PCB* parent = currParent->myPCB;
		child->timeSlice = parent->timeSlice;
		child->myParent = parent;
		globalParent = parent;
		globalChild = child;
		parent->myChildren->addPCB(child);
		PCB::makeChildsStack();

		if (parent->id == PCB::running->id){
			unlock
			return child->id;
		}
		else if (child->id == PCB::running->id){
			//unlock
			return 0;
		}
		parent->myChildren->remove(child);
	}
	unlock
	return childID;
}

void PCB::tellMyParentIFinished() {
	lock
	if (!myParent) {
		unlock
	} else {
		myParent->myChildren->remove(this);
		checkIfParentCanRetire();
		unlock
	}
}

void PCB::checkIfParentCanRetire() {
	if (myParent->myChildren->getFirst() == 0 && myParent->waitingForChildren) {
		myParent->waitingForChildren = 0;
		myParent->state = READY;
		Scheduler::put(myParent);
		myParent = 0;
	}
}

void PCB::tellChildrenIRetired() {
	lock
	this->myChildren->retiredParent();
	unlock
}

void PCB::exit() {
	lock
	PCB::running->clearWaitingToCompletePCBList();
	PCB::running->tellChildrenIRetired();
	PCB::running->tellMyParentIFinished();
	PCB::running->state = FINISHED;
	unlock
	dispatch();
}

void PCB::waitForForkChildren() {
	lock
	if (PCB::running->myChildren->getFirst() == 0) {
		unlock
		return;
	} else {
		PCB::running->waitingForChildren = 1;
		PCB::running->state = BLOCKED;
		unlock
		dispatch();
	}
}
