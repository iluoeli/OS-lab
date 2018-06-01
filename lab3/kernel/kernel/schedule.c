#include "x86.h"

struct TSS tss;

void add_to_queue(struct ProcessTable *m);
void del_to_queue(struct ProcessTable *m);


void schedule()
{
	if(need_resched == 0)	return;

	struct ProcessTable *next;
	next = NULL;
	if(current != NULL && current->state == TASK_RUNNING)	{}
	else if((current == NULL || current->state == TASK_BLOCKED) && run_list){
		next = run_list;
		del_to_queue(next);	
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

