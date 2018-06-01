#ifndef __X86_H__
#define __X86_H__

#include "x86/cpu.h"
#include "x86/memory.h"
#include "x86/io.h"
#include "x86/irq.h"
#include "x86/debug.h"

void initSeg(void);
uint32_t loadUMain(void);

#endif
