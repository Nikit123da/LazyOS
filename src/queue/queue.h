#ifndef QUEUE_H
#define QUEUE_H
#include <stdint.h>

#define MAX_PROCESSES 256
typedef enum {
  PROCESS_UNUSED,
  PROCESS_READY,
  PROCESS_RUNNING,
  PROCESS_TERMINATED
} process_state;

// Keep sp/virt_addr/state at offsets 0/4/8 — context_switch.asm relies on
// these offsets.
typedef struct __attribute__((packed)) {
  uint32_t sp; // current esp (top of saved context)
  void *cr3;   // page directory physical addr / cr3
  process_state state;
  uint8_t priority;
  uint8_t PID;
  float burst_time;
  uint8_t spinning; // 1 while busy-waiting on a lock; scheduler skips burst
} process;

typedef struct {
  process *data[MAX_PROCESSES];
  uint16_t size;
} priority_queue;

void pq_init(priority_queue *q);
void pq_push(priority_queue *q, process *proc);
process *pq_pop(priority_queue *q);
process *pq_peek(priority_queue *q);
int pq_is_empty(priority_queue *q);

#endif
