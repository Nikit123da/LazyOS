#include "pit.h"
#include "../IO/io.h"
#include <stdint.h>

void init_pit(uint32_t frequancy) {
  uint32_t time_slice = 1193182 / frequancy;

  uint8_t mode = 0x34; // 00010100

  outb(MODE_COMMAND_REGISTER, mode);

  outb(PIT_IO_PORT, (uint8_t)(time_slice & 0xFF));
  outb(PIT_IO_PORT, (uint8_t)((time_slice >> 8) & 0xFF));
}
