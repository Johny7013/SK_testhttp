//
// Created by johny on 16.04.20.
//

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include "cookies.h"

cookie make_cookie(char* key, char* val) {
    cookie created_cookie = (cookie) malloc(sizeof(struct Cookie));
    char* cookie_key = (char *) malloc(sizeof(char) * (strlen(key) + 1));
    char* cookie_val = (char *) malloc(sizeof(char) * (strlen(val) + 1));

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

//cookie* init_cookies_array() {
//    cookie* cookies = malloc(sizeof(cookie) * MAX_COOKIES_NUM);
//    return cookies;
//}