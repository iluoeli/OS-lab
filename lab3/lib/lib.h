#ifndef __lib_h__
#define __lib_h__

#include "types.h"

void printf(const char *format,...);
int fork();
int sleep(uint32_t time);
int exit();

#endif
