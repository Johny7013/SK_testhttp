#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <inttypes.h>

#include "cookies.h"
#include "input.h"
#include "http.h"
#include "err.h"
#include "utils.h"

#define BUFFER_SIZE MAX_HEADER_FIELD_LEN + 1

int main(int argc, char* argv[]) {

    if (argc != 4) {
        fatal("Usage: testhttp_raw address:port cookies tested_http_address");
    }

    char* address;
    //uint16_t port;
    char* port;
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

    int sock;
    struct addrinfo addr_hints;
    struct addrinfo *addr_result;

    int i, err;
    char buffer[BUFFER_SIZE];
    char line[BUFFER_SIZE];
    ssize_t len, rcv_len = 1, data_read = 0, read_by_lines, prev_read;
    uint64_t content_len_field = 0, content_len_res = 0;
    bool empty_line_encoutered = false, status_line_encountered = false, status_is_ok = false;

    // 'converting' host/port in string to struct addrinfo
    memset(&addr_hints, 0, sizeof(struct addrinfo));
    addr_hints.ai_family = AF_INET; // IPv4
    addr_hints.ai_socktype = SOCK_STREAM;
    addr_hints.ai_protocol = IPPROTO_TCP;
    err = getaddrinfo(address, port, &addr_hints, &addr_result);
    if (err == EAI_SYSTEM) { // system error
        syserr("getaddrinfo: %s", gai_strerror(err));
    }
    else if (err != 0) { // other error (host not found, etc.)
        fatal("getaddrinfo: %s", gai_strerror(err));
    }

    // initialize socket according to getaddrinfo results
    sock = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);
    if (sock < 0) {
        syserr("socket");
    }

    // connect socket to the server
    if (connect(sock, addr_result->ai_addr, addr_result->ai_addrlen) < 0) {
        syserr("connect");

    }

    freeaddrinfo(addr_result);

    char* http_req_str = http_request_to_str(http_req);

//    len = strnlen(http_req_str, BUFFER_SIZE);
    len = strnlen(http_req_str, BUFFER_SIZE);
    if (len == BUFFER_SIZE) {
        fprintf(stderr, "ignoring long parameter");
    }
    printf("writing to socket: %s\n", http_req_str);
    if (write(sock, http_req_str, (size_t)len) != len) {
        syserr("partial / failed write");
    }

//    memset(buffer, 0, sizeof(buffer));
//
//    rcv_len = read(sock, buffer, sizeof(buffer) - 1);
//    if (rcv_len < 0) {
//        syserr("read");
//    }
//    printf("read from socket: %zd bytes: %s\n", rcv_len, buffer);

    memset(buffer, 0, sizeof(buffer));
    while (rcv_len > 0) {
        memset(buffer, 0, sizeof(buffer));

        rcv_len = read(sock, buffer, sizeof(buffer) - 1);
        if (rcv_len < 0) {
            syserr("read");
        }

        data_read += rcv_len;

        if(empty_line_encoutered) {
            content_len_res += rcv_len;
        }

        if (!empty_line_encoutered) {
            read_by_lines = read_line(buffer, line, BUFFER_SIZE);
            prev_read = 0;

            while (read_by_lines - prev_read > 0 && !empty_line_encoutered) {

                if (!status_line_encountered && !starts_with_prefix(HTTP_OK_RESPONSE, line)) {
                    printf("%s", line);
                    status_is_ok = false;
                }
                else {
                    status_is_ok = true;
                }

                status_line_encountered = true;

                if (status_is_ok && starts_with_prefix("Content-Length: ", line)) {
                    char* con_len;
                    char* val;
                    bisect_string(line, &con_len, &val, ' ');
                    content_len_field = strtoull(val, NULL, 10);
                }

                if (status_is_ok && starts_with_prefix("Set Cookie: ", line)) {
                    //TODO
                }

                if (is_empty_line(line)) {
                    empty_line_encoutered = true;

                    content_len_res += rcv_len - read_by_lines;
                }
                else {
                    prev_read = read_by_lines;
                    read_by_lines += read_line(buffer + prev_read, line, BUFFER_SIZE);
                }
            }
        }

        printf("read from socket: %zd bytes: %s\n", rcv_len, buffer);
    }

    printf("http_len: %" PRIu64 ", my_len: %" PRIu64 "\n", content_len_field, content_len_res);

    printf("Dlugosc zasobu: %" PRIu64 "\n", content_len_res);

    // free
    free(address);
    free_cookies_array(cookies, num_of_cookies);
    free(host);
    free(port);
    free(resource);
    free_header_field_array(header_fields, num_of_http_header_fields);
    free_http_request(http_req);
    free(http_req_str);


    return 0;
}