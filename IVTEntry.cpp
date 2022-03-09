#include "IVTEntry.h"
#include "pcb.h"
#include "timer.h"
#include "kernelEv.h"

IVTEntry* IVTEntry::IVTable[256] = { 0 };

// constructor sets the new routine adress into IV table, and replaces the old one
// the old one is called if that is requested in PREPAREENTRY macro, and is always
// called in the IVTEntry destructor

IVTEntry::IVTEntry(IVTNo ivtNum, intPointer rout)
{

#ifndef BCC_BLOCK_IGNORE
	linkedKernelEvent = 0;
	IVTNumber = ivtNum;
	asm {
		pushf
		cli
	}
	oldRoutine = getvect(IVTNumber);
	setvect(IVTNumber, rout);
	asm {
		popf
	}
	IVTable[IVTNumber] = this;
#endif

}

// the old routine is returned in the entry in this destructor, after calling the old routine

IVTEntry::~IVTEntry()
{
	lock
#ifndef BCC_BLOCK_IGNORE
	asm {
		pushf
		cli
	}
	oldRoutine();
	setvect(IVTNumber, oldRoutine);
	asm {
		popf
	}
	linkedKernelEvent = 0;
	oldRoutine = 0;
#endif
	unlock
}

// signal is just linked to its kernelEvent

void IVTEntry::signal()
{
	if (linkedKernelEvent) linkedKernelEvent->signal();
}
