#ifndef CLOCK_H
#define CLOCK_H

#include "../IO/io.h"
#include <stdint.h>

typedef enum {
  Seconds = 0x00,
  Minutes = 0x02,
  Hours = 0x04,
  Day = 0x07,
  Month = 0x08,
  Year = 0x09,
} t;

typedef struct {
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
  uint8_t day;
  uint8_t month;
  uint8_t year;
} rtc_time;

rtc_time read_date();
void print_time(rtc_time *t);
#endif // !CLOCK_H
