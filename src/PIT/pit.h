#ifndef PIT_H
#define PIT_H
#include <stdint.h>

#define MODE_COMMAND_REGISTER 0x43
#define PIT_IO_PORT 0x40
#define PIT_FREQ 1193182

void init_pit(uint32_t frequancy);
void sleep(uint32_t ms);
void timer_handler();

#endif // !PIT_H
