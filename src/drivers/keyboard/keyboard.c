#include "../../IO/io.h"
#include "../VGA/VGA.h"
#include <stdint.h>
void print_letter(uint8_t scan_code) {
  switch (scan_code) {
  case 0x01:
    print("ESC");
    break;
  case 0x3B:
    print("F1");
    break;
  case 0x3C:
    print("F2");
    break;
  case 0x3D:
    print("F3");
    break;
  case 0x3E:
    print("F4");
    break;
  case 0x3F:
    print("F5");
    break;
  case 0x40:
    print("F6");
    break;
  case 0x41:
    print("F7");
    break;
  case 0x42:
    print("F8");
    break;
  case 0x43:
    print("F9");
    break;
  case 0x44:
    print("F10");
    break;
  case 0x57:
    print("F11");
    break;
  case 0x58:
    print("F12");
    break;

  // Row: Number row
  case 0x29:
    print("`");
    break;
  case 0x02:
    print("1");
    break;
  case 0x03:
    print("2");
    break;
  case 0x04:
    print("3");
    break;
  case 0x05:
    print("4");
    break;
  case 0x06:
    print("5");
    break;
  case 0x07:
    print("6");
    break;
  case 0x08:
    print("7");
    break;
  case 0x09:
    print("8");
    break;
  case 0x0A:
    print("9");
    break;
  case 0x0B:
    print("0");
    break;
  case 0x0C:
    print("-");
    break;
  case 0x0D:
    print("=");
    break;
  case 0x0E:
    Backspace(); // Backspace competability
    break;

  // Row: QWERTY
  case 0x0F:
    print("TAB");
    break;
  case 0x10:
    print("q");
    break;
  case 0x11:
    print("w");
    break;
  case 0x12:
    print("e");
    break;
  case 0x13:
    print("r");
    break;
  case 0x14:
    print("t");
    break;
  case 0x15:
    print("y");
    break;
  case 0x16:
    print("u");
    break;
  case 0x17:
    print("i");
    break;
  case 0x18:
    print("o");
    break;
  case 0x19:
    print("p");
    break;
  case 0x1A:
    print("[");
    break;
  case 0x1B:
    print("]");
    break;
  case 0x2B:
    print("\\");
    break;

  // Row: ASDF
  case 0x3A:
    print("CAPS LOCK");
    break;
  case 0x1E:
    print("a");
    break;
  case 0x1F:
    print("s");
    break;
  case 0x20:
    print("d");
    break;
  case 0x21:
    print("f");
    break;
  case 0x22:
    print("g");
    break;
  case 0x23:
    print("h");
    break;
  case 0x24:
    print("j");
    break;
  case 0x25:
    print("k");
    break;
  case 0x26:
    print("l");
    break;
  case 0x27:
    print(";");
    break;
  case 0x28:
    print("'");
    break;
  case 0x1C:
    print("\n");
    break;
  case 0x2C:
    print("z");
    break;
  case 0x2D:
    print("x");
    break;
  case 0x2E:
    print("c");
    break;
  case 0x2F:
    print("v");
    break;
  case 0x30:
    print("b");
    break;
  case 0x31:
    print("n");
    break;
  case 0x32:
    print("m");
    break;
  case 0x33:
    print(",");
    break;
  case 0x34:
    print(".");
    break;
  case 0x35:
    print("/");
    break;
  case 0x36:
    print("R-SHIFT");
    break;

  case 0x39:
    print(" ");
    break; // Space

  default:
    print("Unknown Key has been pressed\n");
    break;
  }
}

void keyboard_callback() {
  uint8_t scan_code = insb(0x60);
  // checking for the key release event
  if (scan_code & 0x80) {
    outb(0x20, 0x20);
    return;
  }

  print_letter(scan_code);
  outb(0x20, 0x20);
}
