//
// Created by johny on 17.04.20.
//

#include <string.h>
#include <stdlib.h>

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