#include "x86.h"

struct TSS tss;

struct ProcessTable* add_to_queue(struct ProcessTable * list, struct ProcessTable *m);
struct ProcessTable* del_to_queue(struct ProcessTable * list, struct ProcessTable *m);

void schedule()
{
	disableInterrupt();
	if(need_resched == 0)	return;
	need_resched = 0;

	struct ProcessTable *next;
	next = NULL;
	if(current != NULL && current->state == TASK_RUNNING)	{
		need_resched = 1;
	}
	else if((current == NULL || current->state != TASK_RUNNING) && run_list){
		next = run_list;
		run_list = del_to_queue(run_list, next);	
	}
	else{
		next = NULL;
	}
	if(next != NULL){//swithch to next
		tss.esp0 = (uint32_t)(next->stack+MAX_STACK_SIZE - sizeof(struct TrapFrame));	
	
		current = next;
		current->state = TASK_RUNNING;
		if(current->timeCount <= 0)	current->timeCount = 10;
	}
	else current = NULL;

}

