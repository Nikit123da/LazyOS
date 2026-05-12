#include "process.h"
#include "../IO/io.h"
#include "../drivers/VGA/VGA.h"
#include "../memory/heap/kheap.h"
#include "../memory/paging/PMM/pmm.h"
#include "../memory/paging/paging.h"
#include <stdint.h>

process procs[MAX_PROCS];
int indx = 0;

process_queue ready_queue;
process_queue waiting_queue;
uint8_t first_time = true;
process *executing_process;
process *prev_process;
process *done_proc;

extern void context_switch(process *curr, process *next);

void initQueue() {
  queue_init(&ready_queue);
  queue_init(&waiting_queue);
}

process *create_process(
    void (*entry)()) { // get the addr of the the core function of the process
  process *proc = NULL;
  for (int i = 0; i < MAX_PROCS;
       i++) { // NOTE: will be replaced with the PMM functions
    if (procs[i].state == PROCESS_UNUSED) {
      proc = &procs[i];
      break;
    }
  }

  if (!proc)
    return NULL;

  uint32_t *sp = (uint32_t *)&proc->proc_stack[KERNEL_PROCESS_STACK_SIZE];
  *--sp = (uint32_t)entry;
  // *--sp = 0x00000202;
  *--sp = 0; // eax
  *--sp = 0; // ecx
  *--sp = 0; // edx
  *--sp = 0; // ebx
  *--sp = 0; // esp
  *--sp = 0; // ebp
  *--sp = 0; // esi
  *--sp = 0; // edi

  proc->PID = indx++;
  proc->state = PROCESS_READY;
  proc->sp = (uint32_t)sp;
  proc->priority = 1;
  proc->burst_time = 20;

  queue_push(&ready_queue, proc);

  return proc;
}

void scheduler(float time_slice) {
  if (queue_is_empty(&ready_queue) && executing_process == NULL) {
    return;
  }

  if (first_time) {
    first_time = false;
    executing_process = queue_pop(&ready_queue);
    executing_process->state = PROCESS_RUNNING;
    context_switch(NULL, executing_process);
    return;
  }

  prev_process = executing_process;
  prev_process->burst_time -= time_slice;

  if (prev_process->burst_time > 0) {
    prev_process->state = PROCESS_READY;
    queue_push(&ready_queue, prev_process);
  } else {
    prev_process->state = PROCESS_TERMINATED;
    print("process terminated\n");
  }

  if (queue_is_empty(&ready_queue)) {
    print("all done\n");
    executing_process = NULL;
    return;
  }

  executing_process = queue_pop(&ready_queue);
  executing_process->state = PROCESS_RUNNING;
  context_switch(prev_process, executing_process);
}
