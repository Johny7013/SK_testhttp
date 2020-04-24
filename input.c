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
int parse_address_port(const char* input_addr_port, char** address, char** port/*uint16_t* port*/) {
    char* end_ptr;
//    char* port_str;

    if (bisect_string(input_addr_port, address, port/*&port_str*/, ':') == 1 || strlen(*address) == 0 || strlen(*port) == 0) {
        fatal("Wrong syntax in input argument connect address. Correct syntax: address:port");
        return 1;
    }

//    errno = 0;
//
//    *port = (uint16_t)strtoul(port_str, &end_ptr, 10);
//
//    if (errno != 0 || port_str == end_ptr || *port > PORT_NUM_MAX) {
//        fatal("Wrong port number");
//    }
//
//    // if strtoul hasn't read whole port_str
//    if (port_str + strlen(port_str) != end_ptr) {
//        fatal("Wrong value in input argument connect address. Correct syntax: address:port");
//    }
//
//    free(port_str);

}


// remember to free memory allocated for host and resource
// protocol type: 0 - http, 1 - https
void parse_tested_http_address(const char* input_tested_http_addr, char** host, char** resource, int* protocol_type) {
    char http[8] = "http://", https[9] = "https://", http_port[4] = ":80", https_port[5] = ":443";
    size_t http_len = strlen(http), https_len = strlen(https);
    size_t http_port_len = strlen(http_port), https_port_len = strlen(https_port);

    *protocol_type = -1;

    if (starts_with_prefix(http, input_tested_http_addr)) {
        *protocol_type = 0;
        bisect_string(input_tested_http_addr + http_len, host, resource, '/');
    }

    if (starts_with_prefix(https, input_tested_http_addr)) {
        *protocol_type = 1;
        bisect_string(input_tested_http_addr + https_len, host, resource, '/');
    }

    if (*protocol_type == -1) {
        fatal("Wrong tested http address");
    }

    char* host_addr = NULL;
    char* host_port = NULL;

    // when port isn't defined in host name
    if (bisect_string(*host, &host_addr, &host_port, ':') == 1 || strlen(host_port) == 0) {
        size_t host_len = strlen(*host);

        if (*protocol_type == 0) {
            *host = realloc(*host, host_len + http_port_len);

            if (*host == NULL) {
                syserr("Unable to alocate memory");
            }

            strcat(*host, http_port);
        }

        if (*protocol_type == 1) {
            *host = realloc(*host, host_len + https_port_len);

            if (*host == NULL) {
                syserr("Unable to alocate memory");
            }

            strcat(*host, https_port);
        }
    }

    if (host_addr && host_port) {
        free(host_addr);
        free(host_port);
    }
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
    ssize_t characters_read;

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
        if (*num_of_cookies >= 50) {
            fatal("More cookies than accepted max value (max_value = 50)");
        }

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