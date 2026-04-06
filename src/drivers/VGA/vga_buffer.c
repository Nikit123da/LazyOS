#include "../../memory/heap/kheap.h"
#include "../../memory/memory.h"
#include "VGA.h"
#include <stdbool.h>
#include <stdint.h>
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

bool strcomp(char *str1, char *str2) {
  if (strlen(str1) != strlen(str2)) {
    return false;
  }

  while (*str1 != '\0' || *str2 != '\0') {
    if (*str1 != *str2) {
      return false;
    }
    str1++;
    str2++;
  }

  return true;
}

void bufferTestCommand() {
  if (strcomp(buff, "test")) {
    print("testing the buffer\n");
  } else if (strcomp(buff, "clear")) {
    terminal_initialize();
  } else {
    print("Command not recognized");
  }
}

void print_buff() { print(buff); }
