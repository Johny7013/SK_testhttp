#ifndef SK_MALE_ZADANIE1920_COOKIES_H
#define SK_MALE_ZADANIE1920_COOKIES_H

// values set according to RFC6265
// (i.e. minimal values to meet the requirements)
#define MAX_COOKIES_NUM 50
#define MAX_COOKIE_LEN 4096

#include <stddef.h>

struct Cookie;

typedef struct Cookie* cookie;

struct Cookie
{
    char* key;
    char* val;
};

// makes cookie form key and val of this cookie
// allocates memory for cookie, which need to be freed (by free_cookie)
// stores own copies of key and val
//
// returns created cookie - on success , NULL pointer - on failure
cookie make_cookie(const char* key, const char* val);

// frees cookie
void free_cookie(cookie cookie1);

// prints cookie on standard output
void print_cookie(cookie cookie1);

// frees cookie array (i.e. cookie* with cookies on first number_of_cookies positions)
// requires first number_of_cookies positions to be cookies created by make_cookie function
void free_cookies_array(cookie* cookies, size_t num_of_cookies);

// creates string out of cookie1 which is in format "key=val"
// allocates memory for string, which need to be freed (by free function)
//
// returns string "key=val" - on success, NULL pointer - on failure
char* cookie_to_str(cookie cookie1);

// retrive cookie from valid Set-Cookie http header line
// allocates memory for cookie, which need to be freed (by free_cookie)
// checks for end of cookie by searchin for possible-ends of cookie in Set-Cookie line
// possible_ends_of_cookie are set according to RFC2109
//
// returns retrived cookie - on success, NULL pointer - on failure
cookie retrieve_cookie_from_set_cookie(char* set_cookie_line);

#endif //SK_MALE_ZADANIE1920_COOKIES_H
