//
// Created by johny on 16.04.20.
//

#ifndef SK_MALE_ZADANIE1920_COOKIES_H
#define SK_MALE_ZADANIE1920_COOKIES_H

// values set according to RFC6265
// (i.e. minimal values to meet the requirements)
#define MAX_COOKIES_NUM 50
#define MAX_COOKIE_LEN 4096

struct Cookie;

typedef struct Cookie* cookie;

struct Cookie
{
    char* key;
    char* val;
};

cookie make_cookie(char* key, char* val);

void print_cookie(cookie cookie1);


#endif //SK_MALE_ZADANIE1920_COOKIES_H
