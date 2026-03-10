#include "VGA.h"
#include <stddef.h>
#include <stdint.h>

uint16_t *video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, color color) {
  return (color << 8) | c;
} // puts the  color in the second bit, and pluses the char

void terminal_initialize() {
  video_mem = (uint16_t *)(0xB8000);
  terminal_row = 0;
  terminal_col = 0;
  for (int y = 0; y < VGA_HEIGHT; y++) {
    for (int x = 0; x < VGA_WIDTH; x++) {
      terminal_put_char(x, y, Black, 'd');
    }
  }
}

size_t strlen(const char *str) {
  size_t len = 0;
  while (str[len]) {
    len++;
  }

  return len;
}

void terminal_put_char(uint16_t x, uint16_t y, color color, char ch) {
  uint16_t location = (y * VGA_WIDTH) + x;
  video_mem[location] = terminal_make_char(ch, color);
}

void print(char *msg) {
  for (int i = 0; i < strlen(msg); i++) {
    terminal_write_char(msg[i], Cyan);
  }
}

void terminal_write_char(char c, color color) {
  if (c == '\n') { // go down line
    terminal_row += 1;
    terminal_col = 0;
    return;
  }

  if (terminal_col >= VGA_WIDTH) { // end of line
    terminal_col = 0;
    terminal_row += 1;
  }

  terminal_put_char(terminal_col, terminal_row, color, c);

  terminal_col += 1;
}

void terminal_clear() {
  for (int i = 0; i < VGA_HEIGHT; i++) {
    for (int j = 0; j < VGA_WIDTH; j++) {
      terminal_put_char(j, i, Black, ' ');
    }
  }
  terminal_col = 0;
  terminal_row = 0;
}
