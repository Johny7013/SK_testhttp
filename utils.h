// Author: Jan Klinkosz, id number: 394 342

#ifndef SK_MALE_ZADANIE1920_UTILS_H
#define SK_MALE_ZADANIE1920_UTILS_H

#include <stdbool.h>
#include <stdlib.h>

// bisects origin into two strings (first and second).
// first - a string on the left side of the first occurrence of delimiter in origin
// second - a string on the right side of the first occurrence of delimiter in origin
// allocates memory for first and second, which need to be freed (by free function)
//
// returns 0 - on success, -1 - if couldn't find delimiter, -2 - if was unable to allocate memory
int bisect_string(const char* origin, char** first, char** second, char delimiter);

// deletes endline character if last character of str is '\n'
// after this operation length of str is shorter by 1
void delete_following_endl(char** str);

// checks if str starts with prefix (case sensitive)
bool starts_with_prefix(const char* prefix, const char* str);

// checks if str starts with prefix (case insensitive)
bool starts_with_prefix_case_insensitive(const char* prefix, const char* str);

// reads line (sequence of chars ended with '\n') from source and copies it into dest
//
// returns number of bytes in copied line - on success, 0 - on failure
// also sets errno on failure
// if failed to found '\n' in source then errno = 1
// if line read is too long (longer than MAX_HEADER_FIELD_LEN) then errno = 2
ssize_t read_line(const char* source, char* dest, size_t max_line_len);

// checks if line consists only of CRLF
// i.e. it is empty line in terms of http message format
bool is_empty_line(const char* line);

// finds first chracter in str that isn't whitespace
// and returns pointer to that character
//
// returns pointer to first char in str which isn't whitespace
char* pass_whitespaces(const char* str);

// checks if line from http response sets cookie
// by checking if it starts with prefix "Set-Cookie" (case insensitive)
bool line_sets_cookie(const char* line);

// checks if line from http response sets transfer-encoding to chunked
// by checking if it is proper setting transfer encoding in http syntax
bool line_sets_transfer_encoding_chunked(const char* line);

// concatenate str1 with str2 partitioned by delimiter (i.e. creates str1 + delimiter + str2)
// allocates memory for result string, which need to be freed (by free function)
//
// returns composed string - on success, NULL pointer - on failure
char* compose_strings(char* str1, char* str2, char* delimiter);

#endif //SK_MALE_ZADANIE1920_UTILS_H
