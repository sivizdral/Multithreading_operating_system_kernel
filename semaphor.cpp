#include "semaphor.h"
#include "kernSem.h"

Semaphore::Semaphore(int init)
{
	myImpl = new KernelSem(this, init);
}

Semaphore::~Semaphore()
{
	delete myImpl;
}

int Semaphore::wait(Time maxTimeToWait)
{
	if (myImpl) return myImpl->wait(maxTimeToWait);
	return -1;
}

void Semaphore::signal()
{
	if (myImpl) myImpl->signal();
}

int Semaphore::val() const
{
	if (myImpl) return myImpl->value;
	return 0;
}
