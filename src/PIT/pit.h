#ifndef PIT_H
#define PIT_H

#include <stdint.h>

#define MODE_COMMAND_REGISTER 0x43
#define PIT_IO_PORT 0x40

void init_pit(uint32_t frequancy);

#endif // !PIT_H
