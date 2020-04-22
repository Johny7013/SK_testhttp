#ifndef SK_MALE_ZADANIE1920_COOKIES_H
#define SK_MALE_ZADANIE1920_COOKIES_H

// values set according to RFC6265
// (i.e. minimal values to meet the requirements)
#define MAX_COOKIES_NUM 50
#define MAX_COOKIE_LEN 4096

#include <stdlib.h>

struct Cookie;

typedef struct Cookie* cookie;

struct Cookie
{
    char* key;
    char* val;
};
// remember to free cookie
cookie make_cookie(const char* key, const char* val);

void free_cookie(cookie cookie1);

void print_cookie(cookie cookie1);

//cookie* init_cookies_array();

void free_cookies_array(cookie* cookies, size_t num_of_cookies);

//remeber to free str
char* cookie_to_str(cookie cookie1);

cookie retrieve_cookie_from_set_cookie(char* set_cookie_line);

#endif //SK_MALE_ZADANIE1920_COOKIES_H
