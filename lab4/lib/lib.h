#ifndef __lib_h__
#define __lib_h__

#include "types.h"

//#define sem_t uint32_t
struct sem_t {
	int value;
	uint32_t pcb;
};
typedef struct sem_t sem_t;

void printf(const char *format,...);
int fork();
int sleep(uint32_t time);
int exit();

sem_t *sem_creat();
int sem_init(sem_t *sem, uint32_t value);
int sem_post(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_destroy(sem_t *sem);


#endif
