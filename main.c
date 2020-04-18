#include <stdio.h>
#include <stdint.h>

#include "cookies.h"
#include "input.h"
#include "err.h"

int main(int argc, char* argv[]) {

    if (argc != 4) {
        fatal("Usage: testhttp_raw address:port cookies tested_http_address");
    }

    char* address;
    uint16_t port;
    cookie* cookies;
    size_t num_of_cookies;
    char* host;
    char* resource;
    int protocol_type;

    parse_address_port(argv[1], &address, &port);

    read_cookies(argv[2], &cookies, &num_of_cookies);

    parse_tested_http_address(argv[3], &host, &resource, &protocol_type);

    printf("Address: %s, Port: %d \n", address, port);

    printf("\n");
    printf("Cookies: \n");

    for (size_t i = 0; i < num_of_cookies; i++) {
        print_cookie(cookies[i]);
        printf("\n");
    }

    printf("\n");
    printf("Tested_http_address Host: %s; Resource: %s; Protocol_type: %d", host, resource, protocol_type);


    // free
    free(address);
    free_cookies_array(cookies, num_of_cookies);
    free(host);
    free(resource);


    return 0;
}