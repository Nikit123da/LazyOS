#include "pit.h"
#include "../IO/io.h"
#include "../drivers/VGA/VGA.h"
#include "../process/process.h"
#include <stdint.h>
uint32_t time_slice;

void timer_handler() {
  outb(0x20, 0x20);
  scheduler(time_slice);
}

void init_pit(uint32_t freq) {
  time_slice = 1000 / freq;
  int32_t divisor = PIT_FREQ / freq; // = 0x2E9B
  uint8_t mode = 0x36; // 00110110 (00) channel 0 (11) acess mode (011) square
                       // wave generator (0) 16 bit binary
                       // Expect two writes into port 0x40 that takes only 8
  outb(MODE_COMMAND_REGISTER, mode);
  outb(PIT_IO_PORT, (uint8_t)(divisor & 0xFF));
  outb(PIT_IO_PORT, (uint8_t)((divisor >> 8) & 0xFF));
}

void sleep(uint32_t ms) {
  uint32_t count = ms * 10000;
  while (count--) {
    __asm__ __volatile__("nop");
  }
}
