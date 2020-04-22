#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#include "utils.h"
#include "err.h"


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
        syserr("Unable to allocate memory for first in bisect_string");
    }

    *second = (char*) malloc(sizeof(char) * (second_len + 1));

    if (*second == NULL) {
        free(first);
        syserr("Unable to allocate memory for second in bisect_string");
    }

    memcpy(*first, origin, first_len);
    (*first)[first_len] = '\0';

    memcpy(*second, delimiter_position + 1, second_len);
    (*second)[second_len] = '\0';

    return 0;
}

void delete_following_endl(char** str) {
    size_t str_len = strlen(*str);

    if ((*str)[str_len - 1] == '\n') {
        (*str)[str_len - 1] = '\0';
    }
}

bool starts_with_prefix(const char* prefix, const char* str) {
    size_t len_prefix = strlen(prefix), len_str = strlen(str);
    return len_prefix > len_str ? false : memcmp(prefix, str, len_prefix) == 0;
}

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

ssize_t read_line(const char* source, char* dest, size_t max_line_len) {
    char* pos = strchr(source, '\n');

    //printf("Source: %s  Found endl: %s\n", source, pos);

    if (pos == NULL) {
        fprintf(stderr, "No line found"); // TODO delete ths line
        errno = 1;
        return 0;
    }

    if (pos - source + 2 > max_line_len) {
        fprintf(stderr, "Line is too long"); // TODO delete ths line
        errno = 2;
        return 0;
    }

    memcpy(dest, source, pos - source + 1);
    dest[pos - source + 1] = '\0';

    return (ssize_t)strlen(dest);
}

bool is_empty_line(const char* line) {
    bool res = true;

    while (*line != '\0') {
        if (!isspace((int)(*line))) {
            res = false;
            break;
        }
        line++;
    }

    return res;
}

char* pass_whitespaces(const char* str) {
    char* passed_whitespaces_str = (char*)str;

    while (isspace(*passed_whitespaces_str)) {
        passed_whitespaces_str++;
    }

    return passed_whitespaces_str;
}

bool line_sets_cookie(const char* line) {
    return starts_with_prefix_case_insensitive("Set-Cookie:", line);
}

bool line_sets_transfer_encoding_chunked(const char* line) {
    char transfer_encoding[] = "Transfer-Encoding:", chunked[] = "chunked";

    if (!starts_with_prefix_case_insensitive("Transfer-Encoding:", line)) {
        return false;
    }

    char* encoding_type = pass_whitespaces((char *)line + strlen(transfer_encoding));

    return starts_with_prefix_case_insensitive(chunked, encoding_type);
}