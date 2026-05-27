#include "VGA.h"
#include "../../IO/io.h"
#include <stddef.h>
#include <stdint.h>

uint16_t *video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;
uint16_t saved_screen[VGA_HEIGHT * VGA_WIDTH];
uint8_t saved_row;
uint8_t saved_col;

void print_color(char *msg, color fg, color bg) {
  for (int i = 0; i < strlen(msg); i++) {
    terminal_write_char(msg[i], fg, bg);
  }
}

void print(char *msg) {
  for (int i = 0; i < strlen(msg); i++) {
    terminal_write_char(msg[i], White, Black);
  }
}

void print_digit(int num) { terminal_write_char(num + '0', Blue, Purple); }

uint16_t terminal_make_char(char c, color fg, color bg) {
  uint8_t color = (bg << 4) | fg;
  return (color << 8) | c;
} // puts the  color in the second byte, and pluses the char

void terminal_initialize() {
  video_mem = (uint16_t *)(0xB8000);
  terminal_row = 0;
  terminal_col = 0;
  for (int y = 0; y < VGA_HEIGHT; y++) {
    for (int x = 0; x < VGA_WIDTH; x++) {
      terminal_put_char(x, y, Black, Black, ' ');
    }
  }
}

void terminal_put_char(uint16_t x, uint16_t y, color fg, color bg, char ch) {
  uint16_t location = (y * VGA_WIDTH) + x;
  video_mem[location] = terminal_make_char(ch, fg, bg);
}

void terminal_scroll() {
  for (int y = 0; y < VGA_HEIGHT - 1; y++) {
    for (int x = 0; x < VGA_WIDTH; x++) {
      video_mem[y * VGA_WIDTH + x] = video_mem[(y + 1) * VGA_WIDTH + x];
    }
  }
  for (int x = 0; x < VGA_WIDTH; x++) {
    video_mem[(VGA_HEIGHT - 1) * VGA_WIDTH + x] =
        terminal_make_char(' ', Black, Black);
  }
}

void terminal_write_char(char c, color fg, color bg) {
  if (c == '\n') { // go down line
    terminal_row += 1;
    terminal_col = 0;
    if (terminal_row >= VGA_HEIGHT) {
      terminal_scroll();
      terminal_row = VGA_HEIGHT - 1;
    }
    return;
  }

  if (terminal_col >= VGA_WIDTH) { // end of line
    terminal_col = 0;
    terminal_row += 1;
    if (terminal_row >= VGA_HEIGHT) {
      terminal_scroll();
      terminal_row = VGA_HEIGHT - 1;
    }
  }

  terminal_put_char(terminal_col, terminal_row, fg, bg, c);

  terminal_col += 1;
}

void terminal_clear() {
  for (int i = 0; i < VGA_HEIGHT; i++) {
    for (int j = 0; j < VGA_WIDTH; j++) {
      terminal_put_char(j, i, Black, Black, ' ');
    }
  }
  terminal_col = 0;
  terminal_row = 0;
}

static void back_one() {
  if (terminal_col == 0) {
    if (terminal_row == 0)
      return;
    terminal_row -= 1;
    terminal_col = VGA_WIDTH;
  } else {
    terminal_col -= 1;
  }
}

static uint8_t cell_attr(uint16_t x, uint16_t y) {
  return (video_mem[y * VGA_WIDTH + x] >> 8) &
         0xFF; // chek the last 8 bits for fg abd bg
}

void Backspace() {
  if (terminal_col == 0 && terminal_row == 0)
    return;

  back_one();
  while (!(terminal_col == 0 && terminal_row == 0) &&
         cell_attr(terminal_col, terminal_row) == 0) { // if black
    back_one();
  }

  terminal_put_char(terminal_col, terminal_row, Black, Black, ' ');
}

void save_window_content() {
  saved_row = terminal_row;
  saved_col = terminal_col;

  for (int i = 0; i < VGA_HEIGHT; i++) {
    for (int j = 0; j < VGA_WIDTH; j++) {
      saved_screen[i * VGA_WIDTH + j] = video_mem[i * VGA_WIDTH + j];
    }
  }
  terminal_row = 0;
  terminal_col = 0;
}

void enter_editor_mode(char *filename) {
  terminal_clear();
  print_color("Termianl mode :", Green, Black);
  print_color(filename, Cyan, Black);
  print("\n\n");
}

void retrive_saved_window() {
  for (int i = 0; i < VGA_HEIGHT; i++) {
    for (int j = 0; j < VGA_WIDTH; j++) {
      video_mem[i * VGA_WIDTH + j] = saved_screen[i * VGA_WIDTH + j];
    }
  }

  terminal_row = saved_row;
  terminal_col = saved_col;
}
