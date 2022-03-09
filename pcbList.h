#ifndef _pcbList_h_
#define _pcbList_h_

#include "pcb.h"
int syncPrintf(const char *format, ...);

class pcbList {


public:

	typedef struct Element {
		PCB* pcb;
		Element* previous;
		Element* next;
	} Element;

	PCB* getFirst() {
		if (first) return first->pcb;
		else return 0;
	}

	PCB* getLast() {
		if (last) return last->pcb;
		else return 0;
	}

	PCB* getCurrent() {
		if (current) return current->pcb;
		else return 0;
	}

	pcbList() {
		first = 0;
		last = 0;
		current = 0;
	}

	void setCurrentToFirst() {
		current = first;
	}

	int hasNext() {
		if (current->next != 0) return 1;
		return 0;
	}

	int hasCurrent() {
		if (current != 0) return 1;
		return 0;
	}

	void next() {
		current = current->next;
	}

	// adds the given PCB to the list
	void addPCB(PCB* pcb) {
		Element* el = new Element();
		el->next = el->previous = 0;
		el->pcb = pcb;
		if (first == 0) first = last = el;
		else {
			last->next = el;
			el->previous = last;
			last = el;
		}
	}

	// clears the entire list (but doesn't destroy PCBs)
	void clear() {
		current = first;
		while (current != 0) {
			Element* temp = current;
			current = current->next;
			delete temp;
		}
		first = last = current = 0;
	}

	// finds PCB with given ID or returns null if not found
	PCB* findByID(ID id) {
		PCB* pcb = 0;
		for (current = first; current != 0; current = current->next) {
			if (current->pcb->id == id) {
				pcb = current->pcb;
				break;
			}
		}
		return pcb;
	}

	// removes the given PCB from the list
	void remove(PCB* pcb) {
		for (current = first; current != 0; current = current->next) {
			if (current->pcb == pcb) {
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

	void printList() {
		for (current = first; current != 0; current = current->next) {
			// syncPrintf("%d", current->pcb->id);
		}
	}

	void retiredParent() {
		for (current = first; current != 0; current = current-> next) {
			current->pcb->myParent = 0;
		}
		clear();
	}
private:
	Element* first;
	Element* last;
	Element* current;

};

#endif

