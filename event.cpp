#include "event.h"
#include "kernelEv.h"

// event is just a wrapper class, all implementation is left to kernelEvent class

Event::Event(IVTNo ivtNo)
{
	myImpl = new KernelEv(ivtNo);
}

Event::~Event()
{
	delete myImpl;
	myImpl = 0;
}

void Event::wait()
{
	if (myImpl) myImpl->wait();
}

void Event::signal()
{
	if (myImpl) myImpl->signal();
}
