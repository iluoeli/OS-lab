#include "boot.h"
#include <string.h>

#define SECTSIZE 512
#define MAXNUM 200
#define BUFSIZE (SECTSIZE*MAXNUM)

#define NULL ((void *)0)

void bootMain(void) {
	/* 加载内核至内存，并跳转执行 */
	void (*kernel) (void);
	
	ELFHeader *elf;
	ProgramHeader *ph;

	unsigned char buf[BUFSIZE];

    int i=0;
	for(i=0; i < MAXNUM; i++)
		readSect((void *)(buf + i*SECTSIZE), i+1);
	 
	elf = (void *)buf;
	for(i=0; i < elf->phnum; i++){
		ph = (ProgramHeader *)(buf + elf->phoff + i * elf->phentsize);
		int vaddr = ph->vaddr;
		int filesz = ph->filesz;
		memcpy((void *)vaddr, buf+ph->off, filesz);
		memset((void *)(vaddr+filesz), 0, ph->memsz-filesz);
	}
	kernel = (void *)elf->entry;
	kernel();
}

void waitDisk(void) { // waiting for disk
	while((inByte(0x1F7) & 0xC0) != 0x40);
}

void readSect(void *dst, int offset) { // reading a sector of disk
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


