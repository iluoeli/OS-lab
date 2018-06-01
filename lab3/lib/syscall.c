#include "lib.h"
#include "types.h"
//#include <stdlib.h>
/*
 * io lib here
 * 库函数写在这
 */

typedef char * va_list ;

#define _INTSIZEOF(n)	((sizeof(n) + sizeof(int) -1) & ~(sizeof(int)-1))
#define va_start(ap, v)		(ap = (va_list)&v + _INTSIZEOF(v))
#define va_arg(ap, type)	(*(type *)((ap += _INTSIZEOF(type)) - _INTSIZEOF(type)))
#define va_end(ap)	(ap = (va_list)0)

//int vsprintf(char *buf, const char *fmt, va_list args);

int32_t syscall(int num, uint32_t a1,uint32_t a2,
		uint32_t a3, uint32_t a4, uint32_t a5)
{
	int32_t ret = 0;

	/* 内嵌汇编 保存 num, a1, a2, a3, a4, a5 至通用寄存器*/
	/* num--system call number
	** 
	 */
	
	asm volatile("pushl %ebx;\
			pushl %esi;\
			pushl %edi");
	asm volatile("movl %0, %%eax" :: "m"(num));
	asm volatile("movl %0, %%ecx" :: "m"(a1));
	asm volatile("movl %0, %%edx" :: "m"(a2));
	asm volatile("movl %0, %%ebx" :: "m"(a3));
	asm volatile("movl %0, %%esi" :: "m"(a4));
	asm volatile("movl %0, %%edi" :: "m"(a5));

	asm volatile("int $0x80");

	asm volatile("movl %%eax, %0" :"=m"(ret) :);
	asm volatile("popl %edi;\
			popl %esi;\
			popl %ebx");
		
	return ret;
}

void write (void *buf, int len)
{
	int num = 4;
	uint32_t fd = 1;
	uint32_t ptr = (uint32_t)buf;
	uint32_t size = (uint32_t)len;

	syscall(num, ptr, size, fd, 0, 0);
}


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



int vsnprintf(char *buf, const char *format, va_list args)
{
	char *str;
	char *arg;

	for(str=buf; *format; ++format){
		if(*format != '%'){
			*(str++) = *format;
			continue;
		}
		format ++;
		switch(*format){
			case 'x':	
				str += itoa(va_arg(args, int), str, 16);
				break;
			case 'c':
				*(str++) = (unsigned char )va_arg(args, int);
				break;
			case 's':	
				arg = va_arg(args, char *);
				for(; *arg; arg++)
					*(str++) = *arg;
				break;
			case 'd':	
				str += itoa(va_arg(args, int), str, 10);
				break;
			default: *(str++) = *format;
		}

		
	}
	va_end(args);

	return (str-buf);
}



void printf(const char *format,...){
	static char buf[256];
	va_list args;
	va_start(args, format);
	int len = vsnprintf(buf, format, args);
	write(buf, len);
}



int fork()
{	
	int num = 2;
	int ret = syscall(num, 0, 0, 0, 0, 0);
	return ret;
}


int sleep(uint32_t time)
{
	int num = 3;
	int ret = syscall(num, time, 0, 0, 0, 0);
	return ret;
}

int exit()
{
	int num = 1;
	int ret = syscall(num, 0, 0, 0, 0, 0);
	return ret;
}

