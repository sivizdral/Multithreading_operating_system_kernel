#include "queue.h"
#include "pcb.h"
#include "kernSem.h"

int syncPrintf(const char *format, ...);

Queue::Queue() {
		first = current = last = 0;
	}

Queue::~Queue() {
	clear();
}

void Queue::clear() {
	current = first;
	Element* old;
	while (current) {
		old = current;
		current = current->next;
		old->pcb->state = PCB::READY;
		Scheduler::put(old->pcb);
		delete old;
	}
	first = current = last = 0;
}

void Queue::addThread(PCB* pcb, Time time) {
	Element* el = new Element(pcb, time);
	if (first == 0) first = last = el;
	else {
		last->next = el;
		last = el;
	}
}

PCB* Queue::removeThread() {
	Element* ret;
	PCB* pcb;
	if (first == 0) return 0;
	if (first == last) {
		ret = first;
		first = last = 0;
		pcb = ret->pcb;
		delete ret;
		return pcb;
	}
	else {
		ret = first;
		first = first->next;
		pcb = ret->pcb;
		delete ret;
		return pcb;
	}
}

void Queue::decrementThisSemaphore(KernelSem* ksem) {
	Element* previous = 0;
	current = first;
	while (current != 0) {
		if (current->timeToWait == 1) {
			PCB* pcb = current->pcb;
			pcb->state = PCB::READY;
			if (previous == 0) {
				if (last == first) last = 0;
				first = current->next;
				delete current;
				current = first;
			} else {
				if (current == last) last = previous;
				previous->next = current->next;
				delete current;
				current = previous->next;
			}
			Scheduler::put(pcb);
			ksem->value++;
			//syncPrintf("VALUE: %d \\", ksem->value);
		} else if (current->timeToWait > 1) {
			current->timeToWait--;
			previous = current;
			current = current->next;
		} else {
			previous = current;
			current = current->next;
		}
	}
}

void Queue::printQueue() {
	// syncPrintf("LISTA NA SEMAFORU: ");
	for (current = first; current != 0; current = current->next) {
		// syncPrintf(" %d t:%d/ ", current->pcb->id, current->timeToWait);
	}
}

