#include "common.h"
#include "x86.h"
#include "device.h"

void initTimer();
void initPCB();
void initSEM();
struct ProcessTable* initUserProcess(uint32_t entry);
void EnterUserSpace(struct ProcessTable *p);

void kEntry(void) {

	initSerial();// initialize serial port
	initIdt(); // initialize idt
	initIntr(); // iniialize 8259a
	initSeg(); // initialize gdt, tss
	initTimer();
	uint32_t entry = loadUMain(); // load user program, enter user space
	initPCB();
	void initSEM();
	initUserProcess(entry);
	enableInterrupt();
	
	while(1){
		waitForInterrupt();
	}

	while(1);
	assert(0);
}
