#include "../../IO/io.h"
#include "../VGA/VGA.h"
#include <stdint.h>

// https://aeb.win.tue.nl/linux/kbd/scancodes-1.html

char return_letter(uint8_t scan_code) {
  switch (scan_code) {
  // Row: Number row
  case 0x29:
    return '`';
    break;
  case 0x02:
    return '1';
    break;
  case 0x03:
    return '2';
    break;
  case 0x04:
    return '3';
    break;
  case 0x05:
    return '4';
    break;
  case 0x06:
    return '5';
    break;
  case 0x07:
    return '6';
    break;
  case 0x08:
    return '7';
    break;
  case 0x09:
    return '8';
    break;
  case 0x0A:
    return '9';
    break;
  case 0x0B:
    return '0';
    break;
  case 0x0C:
    return '-';
    break;
  case 0x0D:
    return '=';
    break;
  case 0x0E:
    Backspace(); // Backspace competability
    break;

  // Row: QWERTY
  case 0x10:
    return 'q';
    break;
  case 0x11:
    return 'w';
    break;
  case 0x12:
    return 'e';
    break;
  case 0x13:
    return 'r';
    break;
  case 0x14:
    return 't';
    break;
  case 0x15:
    return 'y';
    break;
  case 0x16:
    return 'u';
    break;
  case 0x17:
    return 'i';
    break;
  case 0x18:
    return 'o';
    break;
  case 0x19:
    return 'p';
    break;
  case 0x1A:
    return '[';
    break;
  case 0x1B:
    return ']';
    break;

  // Row: ASDF
  case 0x3A:
    // return"CAPS LOCK"; FIX:
    break;
  case 0x1E:
    return 'a';
    break;
  case 0x1F:
    return 's';
    break;
  case 0x20:
    return 'd';
    break;
  case 0x21:
    return 'f';
    break;
  case 0x22:
    return 'g';
    break;
  case 0x23:
    return 'h';
    break;
  case 0x24:
    return 'j';
    break;
  case 0x25:
    return 'k';
    break;
  case 0x26:
    return 'l';
    break;
  case 0x27:
    return ';';
    break;
  case 0x28:
    return '\'';
    break;
  case 0x1C:
    return '\n';
    break;
  case 0x2C:
    return 'z';
    break;
  case 0x2D:
    return 'x';
    break;
  case 0x2E:
    return 'c';
    break;
  case 0x2F:
    return 'v';
    break;
  case 0x30:
    return 'b';
    break;
  case 0x31:
    return 'n';
    break;
  case 0x32:
    return 'm';
    break;
  case 0x33:
    return ',';
    break;
  case 0x34:
    return '.';
    break;
  case 0x35:
    return '/';
    break;
  case 0x36:
    // return"R-SHIFT"; // FIX:
    break;

  case 0x39:
    return ' ';
    break; // Space

  default:
    return '\0';
    break;
  }

  return '\0';
}

void keyboard_callback() {
  uint8_t scan_code = insb(0x60);
  // checking for the key release event
  if (scan_code & 0x80) {
    outb(0x20, 0x20);
    return;
  }

  char ch = return_letter(scan_code);
  char str[2] = {ch, '\0'};

  print(str);

  if (scan_code == 0x0E) {
    remove_one_from_buffer();
  } else if (ch == '\n') {
    print("inside buffer:");
    print_buff();
    print("\n");
    bufferTestCommand();
    clearBuffer();
  } else if (ch != '\0') {
    inputIntoBuffer(ch);
  }

  // print("\n");
  // print("inside buffer: ");
  // print(buff);
  outb(0x20, 0x20);
}
