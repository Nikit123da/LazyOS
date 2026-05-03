#include "process.h"

extern void switch_to_task(process_control_block *next_thread);

void initialize_multitasking() {
  process_control_block PCB;
  PCB.PID = 0;
  PCB.priority = 5;
  PCB.virt_addr = 0x210000;
  PCB.kernel_stack_top = 0x00;
}
