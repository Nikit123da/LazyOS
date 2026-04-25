#include "../../clock/clock.h"
#include "../VGA/VGA.h"
void test_com() { print("testing the buffer\n"); }

void get_time() {
  rtc_time time = read_date();
  print_time(&time);
}
