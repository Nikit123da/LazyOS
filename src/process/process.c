#include "process.h"
#include "../IO/io.h"
#include "../drivers/VGA/VGA.h"
#include "../memory/heap/kheap.h"
#include "../memory/paging/PMM/pmm.h"
#include "../memory/paging/paging.h"
#include <stdint.h>

extern pointer_to_page_directory *PD_ptr;

process procs[MAX_PROCS] = {0};
process kernel_proc = {0};
process *zombie = NULL;
int indx = 1;

priority_queue ready_queue;
priority_queue waiting_queue;
process *executing_process;
uint8_t lock_t = UNLOCKED;

extern void context_switch(process *curr, process *next);
extern void setLock(void *resouce_addr);
extern void releaseLock(void *resouce_addr);
void free_proc(process *proc);

void initQueue() {
  pq_init(&ready_queue);
  pq_init(&waiting_queue);
  kernel_proc.cr3 = (void *)PD_ptr->directory_pointer;
  kernel_proc.state = PROCESS_READY;
}

static void process_exit(void) {
  process *dying = executing_process;

  if (dying) {
    dying->state = PROCESS_TERMINATED;
  }

  zombie = dying;

  if (pq_is_empty(&ready_queue)) {
    executing_process = NULL;
    context_switch(NULL, &kernel_proc);
    return;
  }

  process *next = pq_pop(&ready_queue);
  next->state = PROCESS_RUNNING;
  executing_process = next;
  context_switch(dying, next);
}

void kill_proc() { process_exit(); }

process *create_process(void (*entry)()) {
  process *proc = NULL;
  for (int i = 0; i < MAX_PROCS; i++) {
    if (procs[i].state == PROCESS_UNUSED) {
      proc = &procs[i];
      break;
    }
  }
  if (!proc)
    return NULL;

  pointer_to_page_directory *pd =
      paging_new_process_directory(PD_ptr->directory_pointer);

  if (!pd)
    return NULL;

  uint8_t flags = PAGING_IS_PRESENT | PAGING_IS_WRITABLE;

  uint32_t size = PAGING_PAGE_SIZE * 2;
  int virt_top = vmm_alloc(pd->directory_pointer, PROC_STACK_VIRT, size,
                           flags); // top of the last frame in the physical

  uint32_t top_page_virt =
      (uint32_t)virt_top - PAGING_PAGE_SIZE; // base of last page
  uint32_t phys_base =
      paging_get_physical(pd->directory_pointer, (void *)top_page_virt);

  if (!phys_base)
    return NULL;

  uint32_t phys_top = phys_base + PAGING_PAGE_SIZE;
  uint32_t *sp = (uint32_t *)phys_top;
  *--sp = (uint32_t)process_exit; // exit trampoline: where entry's ret lands
  *--sp = (uint32_t)entry;
  *--sp = 0; // eax
  *--sp = 0; // ecx
  *--sp = 0; // edx
  *--sp = 0; // ebx
  *--sp = 0; // esp
  *--sp = 0; // ebp
  *--sp = 0; // esi
  *--sp = 0; // edi

  uint32_t used = phys_top - (uint32_t)sp;
  proc->sp = virt_top - used;

  proc->cr3 = (void *)pd->directory_pointer;
  proc->PID = indx++;
  proc->state = PROCESS_READY;
  proc->priority = 1;
  proc->burst_time = 400;
  proc->spinning = 0;

  pq_push(&ready_queue, proc);

  return proc;
}

void start_processes(void) {
  if (pq_is_empty(&ready_queue)) {
    return;
  }
  executing_process = pq_pop(&ready_queue);
  executing_process->state = PROCESS_RUNNING;
  context_switch(&kernel_proc, executing_process);
}

void free_proc(process *proc) {
  free_vmm((uint32_t *)proc->cr3, (void *)PROC_STACK_VIRT, PAGING_PAGE_SIZE);
  kfree(proc->cr3);
}

void scheduler(float time_slice) {
  if (zombie) {
    free_proc(zombie);
    zombie = NULL;
  }

  if (!executing_process) {
    return;
  }

  process *prev = executing_process;
  if (!prev->spinning)
    prev->burst_time -= time_slice;

  if (prev->burst_time <= 0) {
    prev->state = PROCESS_TERMINATED;

    if (pq_is_empty(&ready_queue)) {
      executing_process = NULL;
      context_switch(NULL, &kernel_proc); // no more processes to run
      return;
    }

    process *next = pq_pop(&ready_queue);
    next->state = PROCESS_RUNNING;
    executing_process = next;
    context_switch(NULL, next);
    return;
  }

  if (pq_is_empty(&ready_queue)) {
    return;
  }

  prev->state = PROCESS_READY;
  pq_push(&ready_queue, prev);

  process *next = pq_pop(&ready_queue);
  next->state = PROCESS_RUNNING;
  executing_process = next;
  context_switch(prev, next);
}
