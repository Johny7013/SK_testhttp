//
// Created by johny on 16.04.20.
//

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "input.h"
#include "cookies.h"
#include "err.h"
#include "utils.h"

// TODO change functions to return int, where negative values represent certain errors and then functions with printing communicates accoring to error number

// set address and port on values from input_addr_port string
// remember to free memory allocated for address
void parse_address_port(const char* input_addr_port, char** address, uint16_t* port) {
    char* end_ptr;
    char* port_str;

    if (bisect_string(input_addr_port, address, &port_str, ':') == 1) {
        fatal("Wrong syntax in input argument connect address. Correct syntax: address:port");
    }

    errno = 0;

    *port = (uint16_t)strtoul(port_str, &end_ptr, 10);

    if (errno != 0 || port_str == end_ptr || *port > PORT_NUM_MAX) {
        fatal("Wrong port number");
    }

    // if strtoul hasn't read whole port_str
    if (port_str + strlen(port_str) != end_ptr) {
        fatal("Wrong value in input argument connect address. Correct syntax: address:port");
    }

    free(port_str);
}


// remember to free memory allocated for tested_http_address
void parse_tested_http_address(const char* input_tested_http_addr, char** tested_http_addr) {
    *tested_http_addr = (char *) malloc(sizeof(char)*(strlen(input_tested_http_addr) + 1));

    if (*tested_http_addr == NULL) {
       syserr("Unable to allocate memory for http address");
    }

    strcpy(*tested_http_addr, input_tested_http_addr);
}


void read_cookies(const char* file, cookie** cookies, size_t* num_of_cookies) {
    *num_of_cookies = 0;
    char* cookie_key;
    char* cookie_val;

    *cookies = malloc(sizeof(cookie) * MAX_COOKIES_NUM);

    if (*cookies == NULL) {
        syserr("Unable to allocate memory for cookies");
    }

    size_t max_line_len = MAX_COOKIE_LEN + 1; // because of '\n'
    ssize_t  characters_read;

    char cookie_buf[max_line_len + 1]; // because of '\0'
    char* buf = cookie_buf;
    cookie_buf[max_line_len] = '\0';

    FILE *f = fopen(file, "r");

    if (f == NULL)
    {
        syserr("Unable to open file %s", file);
//        return 1;
    }

    characters_read = getline(&buf, &max_line_len, f);

    while (characters_read != -1) {
        bisect_string(cookie_buf, &cookie_key, &cookie_val, '=');

        delete_following_endl(&cookie_val);

        (*cookies)[(int)*num_of_cookies] = make_cookie(cookie_key, cookie_val);

        free(cookie_key);
        free(cookie_val);

        (*num_of_cookies)++;

        characters_read = getline(&buf, &max_line_len, f);
    }

    if (errno != 0) {
        fatal("Unable to read cookies");
    }

    fclose(f);

}