#include "x86.h"
#include "device.h"
#include <string.h>

#define SYS_WRITE 4
#define SYS_EXIT 1
#define SYS_FORK 2
#define SYS_SLEEP 3

#define G_PORT (0xb800)

SegDesc gdt[NR_SEGMENTS];
TSS tss;

void syscallHandle(struct TrapFrame *tf);

void GProtectFaultHandle(struct TrapFrame *tf);

void TimeBreakHandle(struct TrapFrame *tf);

int puts(char *, int);

void add_to_queue(struct ProcessTable *m);
void del_to_queue(struct ProcessTable *m);
void schedule();
void idle();

void irqHandle(struct TrapFrame *tf) {
	/*
	 * 中断处理程序
	 */
	/* Reassign segment register */

	LOG_LIST();

	if(current != NULL){
		memcpy(current->tf, tf, sizeof(struct TrapFrame));
		LOG_PCB(current);
	}

	switch(tf->irq) {
		case -1:
			break;
		case 0xd:
			GProtectFaultHandle(tf);
			break;
		case 0x20:
			TimeBreakHandle(tf);
			break;
		case 0x80:
			syscallHandle(tf);
			break;
		default:
			assert(0);
	}
	if(need_resched == 1)
		schedule();

	if(current == NULL)	idle();

	LOG_STRING("-------------------\n");
}

static void sys_write(struct TrapFrame *tf)
{
	int fd = (tf->ebx);
	uint32_t segBase = (uint32_t )(gdt[tf->ds >> 3].base_15_0 + (gdt[tf->ds>>3].base_23_16<<16) + (gdt[tf->ds >> 3].base_31_24 << 24));
	assert(tf->edx != 0);
	char *buf = (char *)(tf->ecx + segBase);
	int len = tf->edx;
	if(fd == 1)
		puts(buf, len);
	else 
		assert(0);
}

static void sys_exit(struct TrapFrame *tf)
{
	current->state = TASK_FREE;
	need_resched = 1;
		
	nr_process--;
	return;
}

static void sys_fork(struct TrapFrame *tf)
{
	uint32_t *father_start = (uint32_t *)(gdt[tf->ds >> 3].base_15_0 + (gdt[tf->ds>>3].base_23_16<<16) + (gdt[tf->ds >> 3].base_31_24 << 24));
	uint32_t newSegBase = current_base;
	current_base += 0x400000;
	uint32_t *cd_start = (uint32_t *)newSegBase;
	memcpy(cd_start, father_start, 0x400000);
	struct ProcessTable *newpcb = &pcb[next_pcb++];	
	memcpy(newpcb->stack, current->stack, MAX_STACK_SIZE*sizeof(uint32_t));
	newpcb->tf = (struct TrapFrame *)(newpcb->stack + MAX_STACK_SIZE) - 1;
	memcpy(newpcb->tf, tf, sizeof(struct TrapFrame));
	newpcb->tf->eax = 0;	//child process return 0
	current->tf->eax = 2;			//father process return pid of child
	//creat new gdt 
	int newSegCodeIndex = current_index++;
	int newSegDataIndex = current_index++;
	gdt[newSegCodeIndex] = SEG(STA_X | STA_R, newSegBase, 0x400000, DPL_USER);
	gdt[newSegDataIndex] = SEG(STA_W, newSegBase, 0x400000, DPL_USER);
	newpcb->tf->es = USEL(newSegDataIndex);
	newpcb->tf->ss = USEL(newSegDataIndex);
	newpcb->tf->ds = USEL(newSegDataIndex);
	newpcb->tf->gs = USEL(newSegDataIndex);
	newpcb->tf->cs = USEL(newSegCodeIndex);
	newpcb->state = TASK_RUNNABLE;
	newpcb->timeCount = 10;
	newpcb->pid = 2;
	add_to_queue(newpcb);
	nr_process++;

	return;
}

static void sys_sleep(struct TrapFrame *tf)
{
	current->state = TASK_BLOCKED;
	current->sleepTime = tf->ecx;
	need_resched = 1;
	
	return;
}

void syscallHandle(struct TrapFrame *tf) {
	/* 实现系统调用*/

	LOG_STRING("syscall: nr=0x");
	LOG_NUM(tf->eax, 16);
	LOG_STRING("\n");

	switch(tf->eax){
		case SYS_WRITE: sys_write(tf); break;
		case SYS_FORK: sys_fork(tf); break;
		case SYS_EXIT: sys_exit(tf); break;
		case SYS_SLEEP: sys_sleep(tf); break;
		default: assert(0);
	}
}

void GProtectFaultHandle(struct TrapFrame *tf){
	assert(0);
	return;
}


void TimeBreakHandle(struct TrapFrame *tf)
{
	int i;
	for(i=0; i < MAX_PCB_NUM; i++){
		if(pcb[i].state == TASK_RUNNING){
			pcb[i].timeCount --;
			if(pcb[i].timeCount == 0){
				pcb[i].state = TASK_RUNNABLE;
				add_to_queue(&pcb[i]);
				need_resched = 1;
			}
		}
		else if(pcb[i].state == TASK_BLOCKED){
			pcb[i].sleepTime --;
			if(pcb[i].sleepTime == 0){
				pcb[i].state = TASK_RUNNABLE;
				pcb[i].timeCount = 10;
				add_to_queue(&pcb[i]);
				//need_resched = 1;
			}
			
		}
	}

	return ;
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
