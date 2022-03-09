#include "kernSem.h"
#include "semList.h"
#include "queue.h"
#include "pcb.h"
#include "timer.h"

SemList* KernelSem::semaphoreList = new SemList();

// this is the implementation of a standard semaphore; there is a queue
// of threads that are waiting on it; this queue ought to be time decremented
// every time a tick is made (timer interrupt every 55 ms), except for threads
// that have unlimited waiting time on this semaphore (argument 0 is passed into
// wait method) - they wait until their time comes to be unblocked from the semaphore;
// other threads either wait to be signalled, or for their time to run up

KernelSem::KernelSem(Semaphore* sem, int init)
{
	lock
	thisSemaphore = sem;
	if (init > 0) value = init;
	else value = 0;
	waitingQueue = new Queue();
	semaphoreList->addSemaphore(this);
	unlock
}

KernelSem::~KernelSem()
{
	lock
	semaphoreList->removeSemaphore(this);
	waitingQueue->clear();
	unlock
}

int KernelSem::wait(Time maxTimeToWait)
{
	lock
	int ret = 0;
	if (maxTimeToWait < 0) maxTimeToWait = 0;
	if (--value < 0) {
		if ((PCB*)PCB::running != PCB::idle) {
			PCB::running->state = PCB::BLOCKED;
			waitingQueue->addThread((PCB*)PCB::running, maxTimeToWait);
		}
		unlock
		dispatch();
		ret = PCB::running->signalUnblocking;
		PCB::running->signalUnblocking = 0;
	}
	else {
		ret = 1;
		unlock
	}
	return ret;
}

void KernelSem::signal()
{
	lock
	//syncPrintf("MALI SIGNALI SU S.O.S. ZA LJUBAV\n");
	if (++value <= 0) {
		//syncPrintf("TAJ DIM CIGARETE STO TI SALJEM JA\n");
		PCB* pcb = waitingQueue->removeThread();
		if (pcb == 0) {
			unlock
			return;
		}
		pcb->state = PCB::READY;
		pcb->signalUnblocking = 1;
		Scheduler::put(pcb);
	}
	unlock
}

void KernelSem::decrementAllSemaphores() {
	semaphoreList->decrementAllSemaphores();
}

void KernelSem::decrementThisSemaphore() {
	waitingQueue->decrementThisSemaphore(this);
}
