#include "timer.h"
#include <stdio.h>

int Timer::requestedContextSwitch = 0;
int Timer::counter = 2;
intPointer Timer::old = 0;
int Timer::lockCount = 0;

volatile unsigned int tsp;
volatile unsigned int tss;
volatile unsigned int tbp;

void tick();

void interrupt Timer::timer(...)
{

	if (!requestedContextSwitch) {
		if (counter > 0) counter--;
		KernelSem::decrementAllSemaphores();
	}

	if ((counter == 0 && Timer::lockCount == 0 && PCB::running->timeSlice != 0) || requestedContextSwitch) {

		asm{
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}

		PCB::running->sp = tsp;
		PCB::running->ss = tss;
		PCB::running->bp = tbp;

		if (PCB::running->state == PCB::READY) {
			Scheduler::put((PCB*)PCB::running);
		}
		PCB::running = Scheduler::get();

		if (PCB::running != 0 && PCB::running->state == PCB::READY) {
			//PCB::running->state = PCB::RUNNING;
		}
		else {
			PCB::running = PCB::idle;
		}

		tsp = PCB::running->sp;
		tss = PCB::running->ss;
		tbp = PCB::running->bp;

		counter = PCB::running->timeSlice;
#ifndef BCC_BLOCK_IGNORE
		asm{
			mov sp, tsp   // restore sp
			mov ss, tss
			mov bp, tbp
		}
#endif
	}

	if (!requestedContextSwitch) {
		asm int 60h
		tick();
	}

	requestedContextSwitch = 0;

}

void Timer::initialize()
{
#ifndef BCC_BLOCK_IGNORE
	asm {
		pushf
		cli
	}
	old = getvect(0x08);
	setvect(0x08, timer);
	setvect(0x60, old);
	asm {
		popf
	}
#endif
}

void Timer::restore()
{
#ifndef BCC_BLOCK_IGNORE
	asm {
		pushf
		cli
	}
	setvect(0x08, old);
	asm {
		popf
	}
#endif
}
