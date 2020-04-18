#include <stdio.h>
#include <stdint.h>

#include "cookies.h"
#include "input.h"
#include "http.h"
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

    // parse input

    parse_address_port(argv[1], &address, &port);

    read_cookies(argv[2], &cookies, &num_of_cookies);

    parse_tested_http_address(argv[3], &host, &resource, &protocol_type);

    // test parsing

    printf("Address: %s, Port: %d \n", address, port);

    printf("\n");
    printf("Cookies: \n");

    for (size_t i = 0; i < num_of_cookies; i++) {
        print_cookie(cookies[i]);
        printf("\n");
    }

    printf("\n");
    printf("Tested_http_address Host: %s; Resource: %s; Protocol_type: %d", host, resource, protocol_type);

    // prepare http request

    size_t num_of_http_header_fields = num_of_cookies + 2; //+2 because of host and close headers

    http_header_field* header_fields = malloc(sizeof(http_header_field) * num_of_http_header_fields);

    header_fields[0] = make_header_field("Host", host);
    header_fields[1] = make_header_field("Connection", "close");

    char* cookie_str;

    for (size_t i = 2; i < num_of_http_header_fields; i++) {
        cookie_str = cookie_to_str(cookies[i - 2]);
        header_fields[i] = make_header_field("Cookie", cookie_str);
        free(cookie_str);
    }

    http_request http_req = make_http_request(GET, resource, HTTP_VERSION, header_fields, num_of_http_header_fields);

    //tcp stuff

    // free
    free(address);
    free_cookies_array(cookies, num_of_cookies);
    free(host);
    free(resource);
    free_header_field_array(header_fields, num_of_http_header_fields);


    return 0;
}