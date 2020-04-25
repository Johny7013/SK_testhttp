// Author: Jan Klinkosz, id number: 394 342

#ifndef SK_MALE_ZADANIE1920_READ_INPUT_H
#define SK_MALE_ZADANIE1920_READ_INPUT_H

#include <stdint.h>
#include <stdlib.h>
#include "cookies.h"

// sets address and port on values from input_addr_port string
// allocates memory for address and port, which need to be freed (by free function)
//
// returns 0 - on success, -1 - on failure (for example wrong port number)
int parse_address_port(const char* input_addr_port, char** address, char** port);

// sets host and resource on values from input_tested_http_address string
// also sets protocol type on proper value (0 - http, 1 - https) according to prefix of the address
// allocates memory for host and resource, which need to be freed (by free function)
//
// returns 0 - on success, -1 - on failure
int parse_tested_http_address(const char* input_tested_http_addr, char** host, char** resource, int* protocol_type);

// sets cookies on array of cookies read from file
// sets num_of_cookies on number of read cookies
// allocates memory for cookies array and every single cookie, which need to be freed (by free_cookies_array)
//
// returns 0 - on success, -1 - on failure
int read_cookies(const char* file, cookie** cookies, size_t* num_of_cookies);

#endif //SK_MALE_ZADANIE1920_READ_INPUT_H
