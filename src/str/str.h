#ifndef STR_H
#define STR_H
#include <stdbool.h>
#include <stddef.h>

size_t strlen(const char *str);
bool strcomp(char *str1, char *str2);

bool is_digit(char ch);
int to_numberic_digit(char ch);

int strnlen(const char *ch, int max);
#endif // !
