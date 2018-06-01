#ifndef _DEBUG_H_
#define _DEBUG_H_

#define DEBUG

int putString(char *buf);
int putNum(int k, int hex);
void putTF(struct TrapFrame* tf);
void putPCB(struct ProcessTable *p);
void putList(struct ProcessTable *list);

#ifdef DEBUG
#define LOG_STRING(str)		putString(str)	
#define LOG_NUM(k, hex)		putNum(k, hex)
#define LOG_TF(tf)			putTF(tf)
#define LOG_PCB(p)			putPCB(p)
#define LOG_LIST(list)			putList(list)

#else
#define LOG_STRING(str)		
#define LOG_NUM(k, hex)		
#define LOG_TF(tf)			
#define LOG_PCB(p)			
#define LOG_LIST(list)			

#endif				


#endif
