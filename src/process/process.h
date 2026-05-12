#ifndef PROCESS_H
#define PROCESS_H
#include "../queue/queue.h"
#include <stdint.h>

#define MAX_PROCS 2

process *create_process(void (*entry)());
extern void context_switch(process *curr, process *next);
void initQueue();
void scheduler(float time_slice);

#endif // !PROCESS_H
