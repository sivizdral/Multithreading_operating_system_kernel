#ifndef _ivtentry_h_
#define _ivtentry_h_


#include <dos.h>
class KernelEv;
typedef unsigned char IVTNo;

typedef void interrupt(*intPointer) (...);

// in this macro, a new interrupt routine is made, which just signals the IVTEntry,
// which is also made here; if there is a need, the old routine is called as well

#define PREPAREENTRY(entryNo, shouldOldRoutineBeCalled)\
void interrupt inter##entryNo(...);\
IVTEntry newEntry##entryNo(entryNo, inter##entryNo);\
void interrupt inter##entryNo(...) {\
	newEntry##entryNo.signal();\
	if (shouldOldRoutineBeCalled)\
		newEntry##entryNo.oldRoutine();\
}

// IV table has 256 entries, and this parallel table is made for keeping all IVTEntry class
// objects pointers in it

class IVTEntry {
public:
	KernelEv* linkedKernelEvent;
	IVTNo IVTNumber;
	intPointer oldRoutine;

	static IVTEntry* IVTable[256];

	IVTEntry(IVTNo ivtNum, intPointer rout);
	void signal();
	~IVTEntry();
	

};

#endif
