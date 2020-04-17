//
// Created by johny on 16.04.20.
//

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "input.h"
#include "cookies.h"
#include "err.h"

// set address and port on values from input_addr_port string
// remember to free memory allocated for address
void parse_address_port(char* input_addr_port, char* address, uint16_t* port) {
    size_t addr_len;
    char* colon_position;
    char* end_ptr;
    char* input_addr_port_end = input_addr_port + strlen(input_addr_port);
    colon_position = strchr(input_addr_port, (int)':');

    if (colon_position == NULL) {
        fatal("Wrong syntax in input argument connect address. Correct syntax: address:port");
    }

    *colon_position = '\0';

    addr_len = strlen(input_addr_port);

    address = (char*) malloc(sizeof(addr_len) + 1);

    if (address == NULL) {
        syserr("Unable to allocate memory for address");
    }

    strcpy(address, input_addr_port);

    errno = 0;

    *port = (uint16_t)strtoul(colon_position + 1, &end_ptr, 10);

    if (errno != 0 || colon_position + 1 == end_ptr || *port > PORT_NUM_MAX) {
        fatal("Wrong port number");
    }

    if (input_addr_port_end != end_ptr) {
        fatal("Wrong value in input argument connect address. Correct syntax: address:port");
    }
}


// remember to free memory allocated for tested_http_address
void parse_tested_http_address(char* input_tested_http_addr, char* tested_http_addr) {
    tested_http_addr = (char *) malloc(sizeof(strlen(input_tested_http_addr)) + 1);

    if (tested_http_addr == NULL) {
       syserr("Unable to allocate memory for http address");
    }

    strcpy(tested_http_addr, input_tested_http_addr);
}

//TODO
void read_cookies(char* file, cookie* cookies, size_t* num_of_cookies) {
    cookies = malloc(sizeof(cookie) * MAX_COOKIES_NUM);

    if (cookies == NULL) {
        syserr("Unable to allocate memory for cookies");
    }



}

//TODO write general function to split string with delimiter
