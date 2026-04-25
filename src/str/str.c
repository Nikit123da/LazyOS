#include <stdbool.h>
#include <stddef.h>

size_t strlen(const char *str) {
  size_t len = 0;
  while (str[len] != 0) {
    len++;
  }

  return len;
}

int strnlen(const char *ch, int max) {
  int i = 0;
  for (; i < max; i++) {
    if (ch[i] == 0) {
      break;
    }
  }
  return i;
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

bool is_digit(char ch) { return ch >= 48 && ch <= 57; }

int to_numberic_digit(char ch) { return ch - 48; }
