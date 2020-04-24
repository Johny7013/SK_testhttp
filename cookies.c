#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#include "cookies.h"
#include "utils.h"
#include "err.h"

cookie make_cookie(const char* key, const char* val) {
    cookie created_cookie = (cookie) malloc(sizeof(struct Cookie));

    char* cookie_key = (char *) malloc(sizeof(char) * (strlen(key) + 1));
    char* cookie_val = (char *) malloc(sizeof(char) * (strlen(val) + 1));

    if (created_cookie == NULL || cookie_key == NULL || cookie_val == NULL) {
        syserr("Unable to allocate memory");

        free(created_cookie);
        free(cookie_key);
        free(cookie_val);

        return NULL;
    }

    strcpy(cookie_key, key);
    strcpy(cookie_val, val);

    created_cookie->key = cookie_key;
    created_cookie->val = cookie_val;

    return created_cookie;
}

void free_cookie(cookie cookie1) {
    free(cookie1->key);
    free(cookie1->val);
    free(cookie1);
}

void print_cookie(cookie cookie1) {
    printf("%s=%s", cookie1->key, cookie1->val);
}

void free_cookies_array(cookie* cookies, size_t number_of_cookies) {
    for (size_t i = 0; i < number_of_cookies; i++) {
        free_cookie(cookies[i]);
    }
    free(cookies);
}

char* cookie_to_str(cookie cookie1) {
    size_t key_len = strlen(cookie1->key), val_len = strlen(cookie1->val);
    size_t str_len = key_len + val_len + 2;

    char* str = (char*) malloc(sizeof(char) * str_len);

    if (str == NULL) {
        syserr("Unable to allocate memory for cookie string");
    }

    strcpy(str, cookie1->key);
    str[key_len] = '=';
    strcpy(str + key_len + 1, cookie1->val);

    return str;
}

cookie retrieve_cookie_from_set_cookie(char* set_cookie_line) {
    cookie made_cookie;
    char* cookie_str = pass_whitespaces(set_cookie_line + strlen("Set-Cookie:"));
    char* end_of_cookie_str;

    const size_t possible_ends_size = 4;
    char possible_ends_of_cookie_str[] = ";,\r "; // according to RFC2109 (in this case if I want only first cookie)


    for (size_t i = 0; i < possible_ends_size; i++) {
        end_of_cookie_str = strchr(cookie_str, possible_ends_of_cookie_str[i]);

        if (end_of_cookie_str != NULL) {
            const size_t cookie_from_line_len =  end_of_cookie_str - cookie_str;

            char cookie_from_line[cookie_from_line_len + 1];

            memcpy(cookie_from_line, cookie_str, cookie_from_line_len);
            cookie_from_line[cookie_from_line_len] = '\0';

            char* key;
            char* val;

            bisect_string(cookie_from_line, &key, &val, '=');

            made_cookie = make_cookie(key, val);

            free(key);
            free(val);

            break;
        }
    }


    return made_cookie;
}

//cookie* init_cookies_array() {
//    cookie* cookies = malloc(sizeof(cookie) * MAX_COOKIES_NUM);
//    return cookies;
//}