#include "x86.h"
//#include <string.h>

struct ProcessTable pcb[MAX_PCB_NUM];
struct ProcessTable idle_pcb;
struct ProcessTable *current;//the task is running
struct ProcessTable *run_list;//runnable list
int nr_process;//number of process
int need_resched;//if need reschedule	
struct TSS tss;

int puts(char *buf, int len);

void add_to_queue(struct ProcessTable *m)
{
	struct ProcessTable *pre = run_list;
	if(pre == NULL){
		m->link = NULL;
		run_list = m;
	}
	else{
		for(; !pre->link; pre=pre->link);
		pre->link = m;
		m->link = NULL;
	}
}

void del_to_queue(struct ProcessTable *m)
{
	struct ProcessTable *pre = run_list;
	if(!run_list)	return;
	if(run_list == m){
		run_list = run_list->link;
		pre->link = NULL;
	}
	else{
		for(; pre->link && pre->link != m; pre=pre->link);
		if(pre->link == m){
			struct ProcessTable *tmp = pre->link;
			pre->link = tmp->link;
			tmp->link = NULL;
		}
	}
}

void idle()
{
	if(nr_process != 0)
		enableInterrupt();
	waitForInterrupt();
}

void initPCB()
{
	int i;
	for(i=0; i < MAX_PCB_NUM; i++){
		pcb[i].state = TASK_FREE;
		pcb[i].timeCount = 0;
		pcb[i].sleepTime = 0;
		pcb[i].pid = -1;
	}
	current = NULL;
	run_list = NULL;
	nr_process = 0;
	next_pcb = 0;
	need_resched = 0;//if need reschedule	
}

struct ProcessTable* initUserProcess(uint32_t entry)
{
	int i = next_pcb++;
	pcb[i].state = TASK_RUNNABLE;	
	pcb[i].timeCount = 20;
	pcb[i].sleepTime = 0;
	pcb[i].pid = 1;

	pcb[i].tf = (struct TrapFrame *)(pcb[0].stack + MAX_STACK_SIZE) - 1;
	
	pcb[i].tf->eip = entry;
	pcb[i].tf->cs = USEL(SEG_UCODE);
	pcb[i].tf->eflags = 0x202;
	pcb[i].tf->esp = 0x400000;
	pcb[i].tf->ss = USEL(SEG_UDATA);
	pcb[i].tf->ds = USEL(SEG_UDATA);
	pcb[i].tf->es = USEL(SEG_UDATA);
	add_to_queue(&pcb[i]);
	need_resched = 1;
	nr_process ++;

	return &pcb[0];
}




