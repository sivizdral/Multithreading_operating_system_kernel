#ifndef _kernelev_h_
#define _kernelev_h_

class PCB;
typedef unsigned char IVTNo;

class KernelEv {
public:

	KernelEv(IVTNo IVTNum);
	~KernelEv();

	void wait();
	void signal();

	int value;
	int isBoundedThreadBlocked;
	IVTNo IVTEntryNumber;
	PCB* boundedThread;

};

#endif
