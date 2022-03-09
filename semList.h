#ifndef _semlist_h_
#define _semlist_h_

class KernelSem;

class SemList {
public:
	struct Element {
		KernelSem* sem;
		Element* previous;
		Element* next;
		Element(KernelSem* s) {
			sem = s;
			next = 0;
			previous = 0;
		}
	};

	SemList();
	~SemList();
	void addSemaphore(KernelSem* semaphore);
	void clear();
	void removeSemaphore(KernelSem* semaphore);
	void decrementAllSemaphores();

private:
	Element* first, * last, * current;
};

#endif
