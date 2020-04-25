// Author: Jan Klinkosz, id number: 394 342

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "input.h"
#include "cookies.h"
#include "err.h"
#include "utils.h"

#define MAX_PORT_NUM 65535

// sets address and port on values from input_addr_port string
// allocates memory for address and port, which need to be freed (by free function)
//
// returns 0 - on success, -1 - on failure
int parse_address_port(const char* input_addr_port, char** address, char** port) {

    int bisect_res = bisect_string(input_addr_port, address, port, ':');

    if (bisect_res == -1 || strlen(*address) == 0 || strlen(*port) == 0) {
        fatal("Wrong syntax in input argument connect address. Correct syntax: address:port");
        return -1;
    }

    if (bisect_res == -2) {
        return -1;
    }

    return 0;
}


// sets host and resource on values from input_tested_http_address string
// also sets protocol type on proper value (0 - http, 1 - https) according to prefix of the address
// allocates memory for host and resource, which need to be freed (by free function)
//
// returns 0 - on success, -1 - on failure
int parse_tested_http_address(const char* input_tested_http_addr, char** host, char** resource, int* protocol_type) {
    char http[8] = "http://", https[9] = "https://", http_port[4] = ":80", https_port[5] = ":443";
    size_t http_len = strlen(http), https_len = strlen(https);
    size_t http_port_len = strlen(http_port), https_port_len = strlen(https_port);

    *protocol_type = -1;

    if (starts_with_prefix(http, input_tested_http_addr)) {
        *protocol_type = 0;
        if (bisect_string(input_tested_http_addr + http_len, host, resource, '/') < 0) {
            return -1;
        }
    }

    if (starts_with_prefix(https, input_tested_http_addr)) {
        *protocol_type = 1;
        if (bisect_string(input_tested_http_addr + https_len, host, resource, '/') < 0) {
            return -1;
        }
    }

    if (*protocol_type == -1) {
        fatal("Wrong tested http address");
        return -1;
    }

    char* host_addr = NULL;
    char* host_port = NULL;

    // when port isn't defined in host name set port name according to protocol used
    if (bisect_string(*host, &host_addr, &host_port, ':') == -1 || strlen(host_port) == 0) {
        size_t host_len = strlen(*host);

        if (*protocol_type == 0) {
            *host = realloc(*host, host_len + http_port_len + 1);

            if (*host == NULL) {
                memerr_alloc();
                return -1;
            }

            strcat(*host, http_port);
        }

        if (*protocol_type == 1) {
            *host = realloc(*host, host_len + https_port_len + 1);

            if (*host == NULL) {
                memerr_alloc();
                return -1;
            }

            strcat(*host, https_port);
        }
    }

    if (host_addr && host_port) {
        free(host_addr);
        free(host_port);
    }

    return 0;
}

// sets cookies on array of cookies read from file
// sets num_of_cookies on number of read cookies
// allocates memory for cookies array and every single cookie, which need to be freed (by free_cookies_array)
//
// returns 0 - on success, -1 - on failure
int read_cookies(const char* file, cookie** cookies, size_t* num_of_cookies) {
    *num_of_cookies = 0;
    char* cookie_key;
    char* cookie_val;

    *cookies = malloc(sizeof(cookie) * MAX_COOKIES_NUM);

    if (*cookies == NULL) {
        syserr("Unable to allocate memory for cookies");
        return -1;
    }

    size_t max_line_len = MAX_COOKIE_LEN + 1; // because of '\n'
    ssize_t characters_read;

    char cookie_buf[max_line_len + 1]; // because of '\0'
    char* buf = cookie_buf;
    cookie_buf[max_line_len] = '\0';

    FILE *f = fopen(file, "r");

    if (f == NULL)
    {
        syserr("Unable to open file %s", file);
        goto error_exit;
    }

    errno = 0;

    characters_read = getline(&buf, &max_line_len, f);

    while (characters_read != -1) {
        if (*num_of_cookies >= MAX_COOKIES_NUM) {
            fatal("More cookies than accepted max value (max_value = %d)", MAX_COOKIES_NUM);
            goto error_exit;
        }

        if (bisect_string(cookie_buf, &cookie_key, &cookie_val, '=') < 0) {
            goto error_exit;
        }

        delete_following_endl(&cookie_val);

        cookie made_cookie = make_cookie(cookie_key, cookie_val);

        if (made_cookie == NULL) {
            fatal("Unable to make cookie :(");
            goto error_exit;
        }

        (*cookies)[(int)(*num_of_cookies)] = made_cookie;

        free(cookie_key);
        free(cookie_val);

        (*num_of_cookies)++;

        characters_read = getline(&buf, &max_line_len, f);
    }

    if (errno != 0) {
        fatal("Unable to read cookies from file");
        goto error_exit;
    }

    if (fclose(f) != 0) {
        syserr("Unable to close file");
        goto error_exit;
    }

    return 0;

error_exit:
    free_cookies_array(*cookies, (*num_of_cookies));
    return -1;
}