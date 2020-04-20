#ifndef SK_MALE_ZADANIE1920_UTILS_H
#define SK_MALE_ZADANIE1920_UTILS_H

#include <stdbool.h>
#include <stdlib.h>

// bisects origin into two strings (first and second).
// first - a string on the left side of the first occurrence of delimiter in origin
// second - a string on the right side of the first occurrence of delimiter in origin
// returns 0 if OK, -1 if there is sth wrong, throws error if was unable to allocate memory
// remember to free memory for first and second
int bisect_string(const char* origin, char** first, char** second, char delimiter);

void delete_following_endl(char** str);

bool starts_with_prefix(const char* prefix, const char* str);

ssize_t read_line(char* source, char* dest, size_t max_line_len);

bool is_empty_line(char* line);

#endif //SK_MALE_ZADANIE1920_UTILS_H
