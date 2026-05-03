#ifndef PROCESS_H
#define PROCESS_H
#include <stdint.h>

#define MAX_PROCESSES 12
#define KERNEL_PROCESS_STACK 4096

typedef enum {
  PROCESS_READY,
  PROCESS_WAITING,
  RPROCESS_RUNNING,
  PROCESS_TERMINATED
} process_state;

typedef struct process_control_block {
  void *kernel_stack_top; // esp
  void *virt_addr;
  struct process_control_block *next;
  uint8_t state;
  uint8_t priority;
  uint8_t PID;
} process_control_block;

void initialize_multitasking();

#endif // !PROCESS_H
