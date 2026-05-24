#ifndef PROCESS_H
#define PROCESS_H
#include "../memory/paging/paging.h"
#include "../queue/queue.h"
#include <stdint.h>

#define MAX_PROCS 4
#define LOCKED 1
#define UNLOCKED 0

#define PROC_STACK_VIRT 0x10000000 // every process virt dir

extern process kernel_proc;

process *create_process(void (*entry)());
extern void context_switch(process *curr, process *next);
void initQueue();
void scheduler(float time_slice);
void start_processes(void);
void kill_proc();

extern void setLock(void *resouce_addr);
extern void releaseLock(void *resouce_addr);

#endif // !PROCESS_H
