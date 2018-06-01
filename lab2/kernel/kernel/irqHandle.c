#include "x86.h"
#include "device.h"

#define SYS_WRITE 4

#define G_PORT (0xb800)

void syscallHandle(struct TrapFrame *tf);

void GProtectFaultHandle(struct TrapFrame *tf);

int puts(char *, int);

void irqHandle(struct TrapFrame *tf) {
	/*
	 * 中断处理程序
	 */
	/* Reassign segment register */

	switch(tf->irq) {
		case -1:
			break;
		case 0xd:
			GProtectFaultHandle(tf);
			break;
		case 0x80:
			syscallHandle(tf);
			break;
		default:assert(0);
	}
}

static void sys_write(struct TrapFrame *tf)
{
	int fd = (tf->ebx);
	char *buf = (char *)(tf->ecx);
	int len = tf->edx;
	if(fd == 1)
		puts(buf, len);
	else 
		assert(0);
}


void syscallHandle(struct TrapFrame *tf) {
	/* 实现系统调用*/
	switch(tf->eax){
		case SYS_WRITE: sys_write(tf); break;
		default: assert(0);
	}
	
}

void GProtectFaultHandle(struct TrapFrame *tf){
	assert(0);
	return;
}


int puts(char *buf, int len)
{	
	char *p1 = (char *)0xb8000;
	char *p2 = buf;
	static int off = 640;
	int i;
	for(i=0; i < len; i++){
		if(*p2 == '\n'){
			off = (off/160+1)*160;
			p2 ++;
		}
		else{
			*(p1+off) = *(p2++);
			*(p1+off+1) = 0x0c;
			off += 2;
		}
	}
	return p2-buf;
	
}
