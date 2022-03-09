#ifndef _queue_h_
#define _queue_h_

class PCB;
class KernelSem;
typedef unsigned int Time;
class Queue {
public:
	struct Element {
		PCB* pcb;
		Element* next;
		Time timeToWait;
		Element(PCB* p, Time t) {
			pcb = p;
			timeToWait = t;
			next = 0;
		}
	};
	
	Queue();
	~Queue();
	void clear();
	void addThread(PCB* pcb, Time time);
	PCB* removeThread();
	void decrementThisSemaphore(KernelSem* ksem);
	void printQueue();


private:
	Element* first, *current, *last;
};

#endif
