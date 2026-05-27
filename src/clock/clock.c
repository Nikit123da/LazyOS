#include "clock.h"
#include "../drivers/VGA/VGA.h"
#include <stdint.h>
rtc_time time;

uint8_t rtc_read(uint8_t type) {
  outb(0x70, type);
  return insb(0x71);
}

uint8_t convert_to_num(uint8_t bcd) { return ((bcd >> 4) * 10) + (bcd & 0x0F); }

void rtc_updated() { // waits for the upproveal when the RTC updates fully
  while (rtc_read(0x0A) &
         0x80) { // MSB = UIP (update in progress) if 1 not read, if 0 ready
  }
}

rtc_time read_date() {
  rtc_updated();
  __asm__("cli");

  time.seconds = convert_to_num(rtc_read(Seconds));
  time.minutes = convert_to_num(rtc_read(Minutes));
  time.hours = convert_to_num(rtc_read(Hours));
  time.day = convert_to_num(rtc_read(Day));
  time.month = convert_to_num(rtc_read(Month));
  time.year = convert_to_num(rtc_read(Year));
  __asm__("sti");
  return time;
}

void itoa(uint8_t num, char *buf) {
  if (num == 0) {
    buf[0] = '0';
    buf[1] = '\0';
    return;
  }

  int i = 0;
  char tmp[4]; // max 3 digits for uint8 (255) + null

  while (num > 0) {
    tmp[i++] = '0' + (num % 10); // get digit, convert to ASCII
    num /= 10;
  }

  // reverse it
  int j = 0;
  for (int k = i - 1; k >= 0; k--) {
    buf[j++] = tmp[k];
  }
  buf[j] = '\0';
}

void print_time(rtc_time *t) {
  char buf[4]; // buffer for the time in string format
  itoa(t->hours, buf);
  print(buf);
  print(":");
  itoa(t->minutes, buf);
  print(buf);
  print(":");
  itoa(t->seconds, buf);
  print(buf);
  print(" ");
  itoa(t->day, buf);
  print(buf);
  print("/");
  itoa(t->month, buf);
  print(buf);
  print("/");
  itoa(t->year, buf);
  print(buf);
  print("\n");
}
