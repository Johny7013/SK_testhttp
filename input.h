//
// Created by johny on 16.04.20.
//

#ifndef SK_MALE_ZADANIE1920_READ_INPUT_H
#define SK_MALE_ZADANIE1920_READ_INPUT_H

#include <stdint.h>
#include <stdlib.h>
#include "cookies.h"

#define PORT_NUM_MAX 65535

void parse_address_port(char* input_addr_port, char* address, uint16_t* port);

void parse_tested_http_address(char* input_tested_http_addr, char* tested_http_addr);

void read_cookies(char* file, cookie* cookies, size_t* num_of_cookies);

#endif //SK_MALE_ZADANIE1920_READ_INPUT_H
