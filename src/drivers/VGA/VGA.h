#ifndef VGA_H
#define VGA_H

#include <stddef.h>
#include <stdint.h>
#define VGA_WIDTH                                                              \
  80 // TODO: check why exactly it is supposed to be 80 (before was 167)
#define VGA_HEIGHT 20
typedef enum {
  Black,
  Blue,
  Green,
  Cyan,
  Red,
  Purple,
  Brown,
  Gray,
  Dark_Gray,
  Light_Blue,
  Light_Green,
  Light_Cyan,
  Light_Red,
  Light_Purple,
  Yellow,
  White
} color;
void print(char *msg);
void terminal_initialize();
uint16_t terminal_make_char(char c, color color);
size_t strlen(const char *str);
void print(char *str);
void terminal_put_char(uint16_t x, uint16_t y, color color, char ch);
void terminal_write_char(char c, color color);
void terminal_clear();
#endif // !VGA_H
