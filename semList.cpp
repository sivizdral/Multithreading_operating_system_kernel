#include "kernSem.h"
#include "SemList.h"

int syncPrintf(const char *format, ...);

SemList::SemList() {
		first = last = current = 0;
	}

SemList::~SemList() {
	clear();
}

// adds the given semaphore to the end of the list
void SemList::addSemaphore(KernelSem* semaphore) {
	Element* el = new Element(semaphore);
	if (first == 0) first = last = el;
	else {
		last->next = el;
		el->previous = last;
		last = el;
	}
}

// clears the entire list
void SemList::clear() {
	current = first;
	while (current != 0) {
		Element* temp = current;
		current = current->next;
		delete temp;
	}
	first = last = current = 0;
}

// removes the given semaphore from the list
void SemList::removeSemaphore(KernelSem* semaphore) {
	KernelSem* s = 0;
	for (current = first; current != 0; current = current->next) {
		if (current->sem == semaphore) {
			if (current == first) {
				if (current->next) {
					current->next->previous = 0;
					first = current->next;
					delete current;
					current = 0;
				}
				else {
					delete current;
					first = current = last = 0;
				}
				break;
			}
			else if (current == last) {
				current->previous->next = 0;
				last = current->previous;
				delete current;
				current = 0;
				break;
			}
			else {
				current->previous->next = current->next;
				current->next->previous = current->previous;
				delete current;
				current = 0;
				break;
			}
		}
	}
}

void SemList::decrementAllSemaphores() {
	for (current = first; current != 0; current = current->next) {
		current->sem->decrementThisSemaphore();
	}
}
