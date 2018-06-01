#include "x86.h"


void putChar(char ch); 


int itoa(int k, char *str, int hex)
{
	int i, j, n, base, sign;
	char buf[32];
	if((k < 0) && hex == 10)	{
		sign = -1;
		k = -k;
	}
	else sign = 1;
	unsigned num = (unsigned)k;
	i = 0;	base = hex;
	do{
		n = num % base;
		if(n >= 0 && n <= 9)	buf[i++] = n + '0';
		else	buf[i++] = n + 'a'-10;
	} while((num = num/base) > 0);
	if(sign < 0)	buf[i++] = '-';
	for(j=0; j < i; j++)
		str[j] = buf[i-j-1];
	return i;
}

int putString(char *buf)
{
	char *p = buf;
	while(*p != '\0'){
		putChar(*p);
		p++; 
	}
	return (int)(p-buf);
}

int putNum(int k, int hex)
{
	int i;
	char buf[32];
	int len = itoa(k, buf, hex);
	for(i=0; i < len; i++){
		putChar(buf[i]);
	}
	return len;
}

void putTF(struct TrapFrame* tf)
{
	putString("%ds=0x"); putNum(tf->ds, 16); putString("  ");
	putString("%cs=0x"); putNum(tf->cs, 16); putString("  ");
	putString("%ss=0x"); putNum(tf->ss, 16); putString("\n");
	putString("%eip=0x"); putNum(tf->eip, 16); putString("  ");
	putString("%esp=0x"); putNum(tf->esp, 16); putString("  ");
	putString("irq=0x"); putNum(tf->irq, 16); putString("\n");
}

void putPCB(struct ProcessTable *p)
{
	putTF(p->tf);
	putString("pid=0x"); putNum(p->pid, 16); putString("  ");
	putString("state=0x"); putNum(p->state, 16); putString("  ");
	putString("timeCount=0x"); putNum(p->timeCount, 16); putString("  ");
	putString("sleepTime=0x"); putNum(p->sleepTime, 16); putString("\n");
}

void putList(struct ProcessTable *list)
{
	int i = 0;
	struct ProcessTable *p = list;
	while(p != NULL){
		putString("i="); putNum(i, 10); putString(", ");
		putString("pid="); putNum(p->pid, 10); putString("\n");
		p = p->link;	i++;
	}
}
