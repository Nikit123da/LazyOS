#include "../../memory/heap/kheap.h"
#include "../../memory/memory.h"
#include "../../str/str.h"
#include "VGA.h"
#include "system_commands.h"
static uint8_t position = 0;

char *buff = NULL;

void init_keyboard_buffer() { buff = kmalloc(4096); }

void inputIntoBuffer(char ch) {
  if (position >= 4096) {
    print("The buffer is full");
    return;
  }
  buff[position] = ch;
  position += 1;
}

void clearBuffer() {
  memset(buff, 0, 4096);
  position = 0;
}

void remove_one_from_buffer() {
  if (position == 0)
    return;
  memset(buff + position - 1, 0, 1);
  position -= 1;
}

void bufferTestCommand() {
  if (strcomp(buff, "test")) {
    test_com();
  } else if (strcomp(buff, "clear")) {
    terminal_clear();
  } else if (strcomp(buff, "get-time")) {
    get_time();
  } else {
    print("Command not recognized\n");
  }
}

void print_buff() { print(buff); }
