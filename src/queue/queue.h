#ifndef QUEUE_H
#define QUEUE_H
#include <stdint.h>

#define KERNEL_PROCESS_STACK_SIZE 4096

typedef enum {
  PROCESS_UNUSED,
  PROCESS_READY,
  PROCESS_WAITING,
  PROCESS_RUNNING,
  PROCESS_TERMINATED
} process_state;

typedef struct __attribute__((packed)) {
  uint32_t sp;     // current esp (top of saved context)
  void *virt_addr; // page directory physical addr / cr3
  process_state state;
  uint8_t priority;
  uint8_t PID;
  uint8_t proc_stack[KERNEL_PROCESS_STACK_SIZE]; // stack lives here
  float burst_time;
} process;

typedef struct queue_node {
  process *proc;
  struct queue_node *next;
} queue_node;

typedef struct {
  queue_node *head; // next to run
  queue_node *tail; // last in line
  uint8_t size;
} process_queue;

// operations
void queue_init(process_queue *q);
void queue_push(process_queue *q, process *proc);
process *queue_pop(process_queue *q);
process *queue_peek(process_queue *q);
uint8_t queue_is_empty(process_queue *q);

#endif
