#include "pit.h"
#include "../IO/io.h"
#include "../drivers/VGA/VGA.h"
#include <stdint.h>
uint32_t ticks;

void timer_handler() {
  ticks += 1;
  print("Timer Has Ticked! ");
}

void init_pit(uint32_t freq) {
  ticks = 0;
  uint32_t time_slice = 1193180 / freq;
  uint8_t mode = 0x36; // 00110110

  outb(MODE_COMMAND_REGISTER, mode);
  outb(PIT_IO_PORT, (uint8_t)(time_slice & 0xFF));
  outb(PIT_IO_PORT, (uint8_t)((time_slice >> 8) & 0xFF));
}
