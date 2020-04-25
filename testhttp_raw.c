// Author: Jan Klinkosz, id number: 394 342

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>

#include "cookies.h"
#include "input.h"
#include "http.h"
#include "err.h"
#include "utils.h"
#include "tcp.h"

#define BUFFER_SIZE MAX_HEADER_FIELD_LEN + 1

//#define BUFFER_SIZE 100 + 1

int main(int argc, char* argv[]) {

    if (argc != 4) {
        fatal("Usage: testhttp_raw address:port cookies tested_http_address");
        return 0;
    }

    char* address;
    char* port;
    cookie* cookies;
    size_t num_of_cookies;
    char* host;
    char* resource;
    int protocol_type;

    // parse input

    if (parse_address_port(argv[1], &address, &port) < 0) {
        fatal("Failed trying to parse address:port form input");
        goto error_exit;
    }

    if (read_cookies(argv[2], &cookies, &num_of_cookies) < 0) {
        fatal("Failed trying to read cookies form file");
        goto clean_before_cookies_array;
    }

    if (parse_tested_http_address(argv[3], &host, &resource, &protocol_type) < 0) {
        fatal("Failed trying to parse tested_http_address form input");
        goto clean_before_tested_http_address;
    }

    // test parsing

    printf("Address: %s, Port: %s \n", address, port);

    printf("\n");
    printf("Cookies: \n");

    for (size_t i = 0; i < num_of_cookies; i++) {
        print_cookie(cookies[i]);
        printf("\n");
    }

    printf("\n");
    printf("Tested_http_address Host: %s; Resource: %s; Protocol_type: %d\n", host, resource, protocol_type);

    // prepare http request

    size_t num_of_http_header_fields;

    http_header_field* header_fields = generate_header_fields(cookies, num_of_cookies, host, &num_of_http_header_fields);

    if (header_fields == NULL) {
        fatal("Failed trying to generate header fields for http request");
        goto clean_before_header_fields;
    }

    http_request http_req = make_http_request(GET, resource, HTTP_VERSION11, header_fields, num_of_http_header_fields);

    if (http_req == NULL) {
        fatal("Failed trying to generate http request");
        goto clean_before_http_request;
    }

    //tcp

    int sock;

    sock = connect_with_server(address, port);

    if (sock < 0) {
        fatal("Failed trying to connect to server");
        goto clean_before_sock;
    }

    if (send_http_request(sock, http_req) < 0) {
        fatal("Failed trying to send http request to server");
        goto clean_all;
    }

    uint64_t content_len_result;

    if (handle_http_response(sock, BUFFER_SIZE, &content_len_result) == 0) {
        printf("Dlugosc zasobu: %" PRIu64 "\n", content_len_result);
    }

    // free standard
    (void) close(sock);
    free_http_request(http_req);
    free_header_field_array(header_fields, num_of_http_header_fields);
    free(resource);
    free(host);
    free_cookies_array(cookies, num_of_cookies);
    free(port);
    free(address);

    return 0;

    // free when error
clean_all:
    (void) close(sock);

clean_before_sock:
    free_http_request(http_req);

clean_before_http_request:
    free_header_field_array(header_fields, num_of_http_header_fields);

clean_before_header_fields:
    free(resource);
    free(host);

clean_before_tested_http_address:
    free_cookies_array(cookies, num_of_cookies);

clean_before_cookies_array:
    free(port);
    free(address);

error_exit:
    exit(EXIT_FAILURE);
}