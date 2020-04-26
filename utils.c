// Author: Jan Klinkosz, id number: 394 342

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#include "utils.h"
#include "err.h"

// bisects origin into two strings (first and second).
// first - a string on the left side of the first occurrence of delimiter in origin
// second - a string on the right side of the first occurrence of delimiter in origin
// allocates memory for first and second, which need to be freed (by free function)
//
// returns 0 - on success, -1 - if couldn't find delimiter, -2 - if was unable to allocate memory
int bisect_string(const char* origin, char** first, char** second, char delimiter) {
    char* delimiter_position;
    delimiter_position = strchr(origin, (int)delimiter);

    if (delimiter_position == NULL) {
        return -1;
    }

    size_t origin_len = strlen(origin);
    size_t first_len = delimiter_position - origin;
    size_t second_len = origin_len - first_len - 1;

    *first = (char*) malloc(sizeof(char) * (first_len + 1));

    if (*first == NULL) {
        memerr_alloc();
        return -2;
    }

    *second = (char*) malloc(sizeof(char) * (second_len + 1));

    if (*second == NULL) {
        free(first);
        memerr_alloc();
        return -2;
    }

    memcpy(*first, origin, first_len);
    (*first)[first_len] = '\0';

    memcpy(*second, delimiter_position + 1, second_len);
    (*second)[second_len] = '\0';

    return 0;
}

// deletes endline character if last character of str is '\n'
// after this operation length of str is shorter by 1
void delete_following_endl(char** str) {
    size_t str_len = strlen(*str);

    if ((*str)[str_len - 1] == '\n') {
        (*str)[str_len - 1] = '\0';
    }
}

// checks if str starts with prefix
bool starts_with_prefix(const char* prefix, const char* str) {
    size_t len_prefix = strlen(prefix), len_str = strlen(str);
    return len_prefix > len_str ? false : memcmp(prefix, str, len_prefix) == 0;
}

// checks if str starts with prefix (case insensitive)
bool starts_with_prefix_case_insensitive(const char* prefix, const char* str) {
    size_t shift = 0;
    bool is_prefix = true;

    while (*(prefix + shift) != '\0' && *(str + shift) != '\0') {
        if (tolower(*(prefix + shift)) != tolower(*(str + shift))) {
            is_prefix = false;
            break;
        }
        shift++;
    }

    // if str is shorter than prefix
    if (*(str + shift) == '\0' && *(prefix + shift) != '\0') {
        is_prefix = false;
    }

    return is_prefix;
}

// reads line (sequence of chars ended with '\n') from source and copies it into dest
//
// returns number of bytes in copied line - on success, 0 - on failure
// also sets errno on failure
// if failed to found '\n' in source then errno = 1
// if line read is too long (longer than MAX_HEADER_FIELD_LEN) then errno = 2
ssize_t read_line(const char* source, char* dest, size_t max_line_len) {
    char* pos = strchr(source, '\n');

    // No line found
    if (pos == NULL) {
        errno = 1;
        return 0;
    }

    if (pos - source + 2 > max_line_len) {
        fatal("Line in http respone is too long");
        errno = 2;
        return 0;
    }

    memcpy(dest, source, pos - source + 1);
    dest[pos - source + 1] = '\0';

    return (ssize_t)strlen(dest);
}

// checks if line consists only of CRLF
// i.e. it is empty line in terms of http message format
bool is_empty_line(const char* line) {

    return strlen(line) == 2 && line[0] == '\r' && line[1] == '\n';
}

// finds first chracter in str that isn't whitespace
// and returns pointer to that character
//
// returns pointer to first char in str which isn't whitespace
char* pass_whitespaces(const char* str) {
    char* passed_whitespaces_str = (char*)str;

    while (isspace(*passed_whitespaces_str)) {
        passed_whitespaces_str++;
    }

    return passed_whitespaces_str;
}

// checks if line from http response sets cookie
// by checking if it starts with prefix "Set-Cookie" (case insensitive)
bool line_sets_cookie(const char* line) {
    return starts_with_prefix_case_insensitive("Set-Cookie:", line);
}

// checks if line from http response sets transfer-encoding to chunked
// by checking if it is proper setting transfer encoding in http syntax
bool line_sets_transfer_encoding_chunked(const char* line) {
    char transfer_encoding[] = "Transfer-Encoding:", chunked[] = "chunked";

    if (!starts_with_prefix_case_insensitive("Transfer-Encoding:", line)) {
        return false;
    }

    char* encoding_type = pass_whitespaces((char *)line + strlen(transfer_encoding));

    return starts_with_prefix_case_insensitive(chunked, encoding_type);
}

// concatenate str1 with str2 partitioned by delimiter (i.e. creates str1 + delimiter + str2)
// allocates memory for result string, which need to be freed (by free function)
//
// returns composed string - on success, NULL pointer - on failure
char* compose_strings(char* str1, char* str2, char* delimiter) {
    size_t str1_len = strlen(str1), str2_len = strlen(str2), delimiter_len = strlen(delimiter);
    char* composed_string = (char*) malloc(sizeof(char) * (str1_len + str2_len + delimiter_len + 1));

    if (composed_string == NULL) {
        memerr_alloc();
        return NULL;
    }

    strcpy(composed_string, str1);
    strcat(composed_string, delimiter);
    strcat(composed_string, str2);

    return composed_string;
}