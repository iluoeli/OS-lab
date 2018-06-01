#include "x86.h"

struct ProcessTable* add_to_queue(struct ProcessTable * list, struct ProcessTable *m);
struct ProcessTable* del_to_queue(struct ProcessTable * list, struct ProcessTable *m);

void add_to_pcb(PCB *p, int n)
{
	if(p != NULL){
		sem[n].next = -1;
		if(p->sem_list < 0){
			p->sem_list = n;
		}
		else {
			int prev = p->sem_list;;
			while(sem[prev].next >= 0){
				prev = sem[prev].next;
			}
			sem[prev].next = n;
		}
	}
}
void del_to_pcb(PCB *p, int n)
{
	if(p != NULL){
		if(p->sem_list < 0)	return;
		else if (p->sem_list == n)	p->sem_list = -1;
		int prev = p->sem_list;
		while(sem[prev].next >= 0 && sem[prev].next != n){
				prev = sem[prev].next;
		}
		if(sem[prev].next == n){
			sem[prev].next = sem[n].next;
			sem[n].next = -1;
		}
	}	
}

PCB *sleep(PCB *list)
{
	list = add_to_queue(list, current);
	current->state = TASK_BLOCKED;
	need_resched = 1;

	return list;
}

PCB *wakeup(PCB *list)
{
	PCB *p = list;
	list = del_to_queue(list, list);
	p->state = TASK_RUNNABLE;
	run_list = add_to_queue(run_list, p);
	return list;
}

void P(Semaphore *s)
{
	s->value --;
	if(s->value < 0)
		s->list = sleep(s->list);
}

void V(Semaphore *s)
{
	s->value ++;
	if(s->value <= 0)
		s->list = wakeup(s->list);
}


void sem_creat(struct TrapFrame *tf)
{
	current->tf->eax = (uint32_t)(&sem);
}

void sem_init(struct TrapFrame *tf)
{
	int num = next_sem % MAX_SEM_NUM;//(Semaphore *)(tf->ecx);
	uint32_t value = (tf->edx);
	Semaphore *s = &sem[num];//(Semaphore *)(tf->ecx);
	next_sem ++;
	s->state = SEM_USED;
	s->linkCount = 1;
	s->value = value;
	LOG_NUM(s->value, 10);
	*(uint32_t *)(GDT_BASE((current->tf->ds) >> 3) + tf->ecx) = num;
	add_to_pcb(current, num);
	if(s->value == value)
		current->tf->eax = 0;
	else current->tf->eax = -1;
}

void sem_post(struct TrapFrame *tf)
{
	//Semaphore *s = (Semaphore *)(tf->ecx);
	Semaphore *s = &sem[tf->ecx];
	V(s);	
	current->tf->eax = 0;
}

void sem_wait(struct TrapFrame *tf)
{
	//Semaphore *s = (Semaphore *)(tf->ecx);
	Semaphore *s = &sem[tf->ecx];
	P(s);	
	current->tf->eax = 0;
}

void sem_destroy(struct TrapFrame *tf)
{
//	Semaphore *s = (Semaphore *)(tf->ecx);
	Semaphore *s = &sem[tf->ecx];
	s->linkCount --;
	del_to_pcb(current, tf->ecx);
	if(s->linkCount == 0)	s->state = SEM_VALID;
	//del s
	current->tf->eax = 0;
}

void initSEM()
{
	int i;
	for(i=0; i < MAX_SEM_NUM; i++){
		sem[i].value = 0;
		sem[i].list = NULL;
		sem[i].state = SEM_VALID;
		sem[i].linkCount = 0;
		sem[i].next = -1;
	}
	next_sem = 0;
}
