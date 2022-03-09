#include "kernelEv.h"
#include "pcb.h"
#include "IVTEntry.h"
#include "timer.h"

// all event implementation is basically here; the running thread is linked
// to one and only one kernelEvent, as it is 1-1-1 function - for one event,
// there can be only one thread, and one event is linked to only one IVTEntry
// event is a binary semaphore, which is used to signal when there is an
// interrupt from an input device (keyboard)

KernelEv::KernelEv(IVTNo IVTNum)
{
	lock
    IVTEntryNumber = IVTNum;
    boundedThread = (PCB*)PCB::running;
    isBoundedThreadBlocked = 0;
    IVTEntry::IVTable[IVTEntryNumber]->linkedKernelEvent = this;
    value = 0;
    unlock
}

KernelEv::~KernelEv()
{
	lock
    if (isBoundedThreadBlocked) {
        boundedThread->state = PCB::READY;
        Scheduler::put(boundedThread);
    }
    IVTEntry::IVTable[IVTEntryNumber]->linkedKernelEvent = 0;
    isBoundedThreadBlocked = 0;
    boundedThread = 0;
    unlock
}

void KernelEv::wait()
{
	if ((PCB*) PCB::running != boundedThread) return;
	lock
	if (value == 0) {
		isBoundedThreadBlocked = 1;
		boundedThread->state = PCB::BLOCKED;
		unlock
		dispatch();
	}
	else {
		value = 0;
		unlock
	}
}

void KernelEv::signal()
{
	lock
	if (isBoundedThreadBlocked == 0) {
		value = 1;
	} else {
		isBoundedThreadBlocked = 0;
		boundedThread->state = PCB::READY;
		Scheduler::put(boundedThread);
	}
	unlock
}
