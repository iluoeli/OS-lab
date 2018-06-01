#include "x86.h"
#include "device.h"
#include <string.h>

SegDesc gdt[NR_SEGMENTS];
TSS tss;

#define SECTSIZE 512
#define NUM 200
#define BUFSIZE (SECTSIZE*NUM)

void waitDisk(void) {
	while((inByte(0x1F7) & 0xC0) != 0x40); 
}

void readSect(void *dst, int offset) {
	int i;
	waitDisk();
	
	outByte(0x1F2, 1);
	outByte(0x1F3, offset);
	outByte(0x1F4, offset >> 8);
	outByte(0x1F5, offset >> 16);
	outByte(0x1F6, (offset >> 24) | 0xE0);
	outByte(0x1F7, 0x20);

	waitDisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = inLong(0x1F0);
	}
}

void initSeg() {
	gdt[SEG_KCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_KERN);
	gdt[SEG_KDATA] = SEG(STA_W,         0,       0xffffffff, DPL_KERN);
	gdt[SEG_UCODE] = SEG(STA_X | STA_R, 0x400000,       0xffffffff, DPL_USER);
	gdt[SEG_UDATA] = SEG(STA_W,         0x400000,       0xffffffff, DPL_USER);
	gdt[SEG_TSS] = SEG16(STS_T32A,      &tss, sizeof(TSS)-1, DPL_KERN);
	gdt[SEG_TSS].s = 0;

	setGdt(gdt, sizeof(gdt));

	/*
	 * 初始化TSS
	 */
	current_index = SEG_TSS + 1;
	current_base = 0x800000;

	tss.ss0 = KSEL(SEG_KDATA);
	tss.esp0 = 0x400000;
	
	asm volatile("ltr %%ax":: "a" (KSEL(SEG_TSS)));

	/*设置正确的段寄存器*/
	asm volatile("movw %%ax, %%es":: "a"(KSEL(SEG_KDATA)));	//es
	asm volatile("movw %%ax, %%ds":: "a"(KSEL(SEG_KDATA)));	//ds
	asm volatile("movw %%ax, %%ss":: "a"(KSEL(SEG_KDATA)));	//ss

	lLdt(0);
	
}

void enterUserSpace(uint32_t entry) {
	/*
	 * Before enter user space 
	 * you should set the right segment registers here
	 * and use 'iret' to jump to ring3
	 */
	uint32_t eflages = 0x00000002;
	
	asm volatile("movw %%ax, %%es":: "a"(USEL(SEG_UDATA)));	//es
	asm volatile("movw %%ax, %%ds":: "a"(USEL(SEG_UDATA)));	//ds

	asm volatile("pushw %0":: "i"(USEL(SEG_UDATA)));		//ss
	asm volatile("pushl %0" :: "i"(128<<10));				//esp

	//iret will pop eip, cs, eflages 
	asm volatile("pushl %0" : : "m"(eflages));				//eflages
	asm volatile("pushl %0":: "i"(USEL(SEG_UCODE)));		//cs	
	asm volatile("pushl %0" : : "m"(entry));				//eip

	asm volatile("iret");
}


uint32_t loadUMain() {

	/*加载用户程序至内存*/
	ELFHeader *elf;
	ProgramHeader *ph = NULL;
	char *p;

	uint8_t buf[BUFSIZE];
	int i;
	for(i=0; i < NUM; i++)
		readSect((void *)(buf+i*SECTSIZE), 201+i);

	elf = (void *)buf;
	const uint32_t elf_magic = 0x464c457f;
	uint32_t *p_magic = (void *)buf;
	assert(*p_magic == elf_magic);

	for(i=0; i < elf->phnum; i++){
		ph = (ProgramHeader *)(buf + elf->phoff + elf->phentsize*i);
		if(ph->type == PT_LOAD){
			p = (char *)ph->vaddr+0x400000;
			memcpy(p, buf+ph->off, ph->filesz);
			memset((void *)(p+ph->filesz), 0, ph->memsz-ph->filesz);
		}
	}
	assert(elf->entry == 0x200000);
	return elf->entry;
}




