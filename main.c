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

    size_t num_of_http_header_fields = num_of_cookies + 2; //+2 because of host and connection headers

    http_header_field* header_fields = malloc(sizeof(http_header_field) * num_of_http_header_fields);

    header_fields[0] = make_header_field("Host", host);
    header_fields[1] = make_header_field("Connection", "close");
//    header_fields[1] = make_header_field("transfer-coding", "chunked");
//    header_fields[1] = make_header_field("User-Agent", "curl/7.58.0");
//    header_fields[2] = make_header_field("Accept", "*/*");
    //header_fields[2] = make_header_field("User-Agent", "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:75.0) Gecko/20100101 Firefox/75.0");

    char* cookie_str;

    for (size_t i = 2; i < num_of_http_header_fields; i++) {
        cookie_str = cookie_to_str(cookies[i - 2]);
        header_fields[i] = make_header_field("Cookie", cookie_str);
        free(cookie_str);
    }

    http_request http_req = make_http_request(GET, resource, HTTP_VERSION11, header_fields, num_of_http_header_fields);

    //tcp stuff

    int sock;

//    char buffer[BUFFER_SIZE];
//    char line[BUFFER_SIZE];
//    size_t chars_left_in_line = 0, still_to_read_in_chunk = 0;
//    ssize_t rcv_len = 1, read_by_lines, prev_read, read_in_line;
//    uint64_t content_len_field = 0, content_len_res = 0, chunk_len = 0;
//    bool empty_line_encoutered = false, status_line_encountered = false, status_is_ok = false;
//    bool encoding_is_chunked = false, zero_chunk_encountered = false;
//    char* end_ptr;

    sock = connect_with_server(address, port);

    send_http_request(sock, http_req);

//    memset(buffer, 0, sizeof(buffer));
//
//    rcv_len = read(sock, buffer, sizeof(buffer) - 1);
//    if (rcv_len < 0) {
//        syserr("read");
//    }
//    printf("read from socket: %zd bytes: %s\n", rcv_len, buffer);

//    memset(buffer, 0, sizeof(buffer));
//
//    while (rcv_len > 0) {
//
//        //read
//        rcv_len = read_from_socket(sock, buffer, BUFFER_SIZE - 1);
//
//        if (rcv_len < 0) {
//            syserr("JOLO");
//        }
//
//        if(empty_line_encoutered && !encoding_is_chunked) {
//            content_len_res += rcv_len;
//        }
//
//        prev_read = 0;
//
//        if (!empty_line_encoutered) {
//
//            read_by_lines = read_line(buffer, line + chars_left_in_line, BUFFER_SIZE - chars_left_in_line);
//
//            chars_left_in_line = 0;
//
//            while (read_by_lines - prev_read > 0 && !empty_line_encoutered) {
//
//                printf("Line: %s", line);
//
//                if (!status_line_encountered) {
//                    status_is_ok = is_status_ok(line);
//                    if (!status_is_ok) {
//                        printf("%s", line);
//                    }
//
//                    status_line_encountered = true;
//                }
//
//                if (status_is_ok) {
//                    if (starts_with_prefix("Content-Length:", line)) {
//                        char* con_len;
//                        char* val;
//                        bisect_string(line, &con_len, &val, ':');
//                        content_len_field = strtoull(val, NULL, 10);
//                    }
//
//                    if (line_sets_cookie(line)) {
//                        cookie retrieved_cookie = retrieve_cookie_from_set_cookie(line);
//                        print_cookie(retrieved_cookie);
//                        free_cookie(retrieved_cookie);
//                    }
//
//                    if (line_sets_transfer_encoding_chunked(line)) {
//                        encoding_is_chunked = true;
//                    }
//                }
//
//                prev_read = read_by_lines; // set how many characters you already read
//
//                if (is_empty_line(line)) {
//                    empty_line_encoutered = true;
//
//                    if (!encoding_is_chunked) {
//                        content_len_res += rcv_len - read_by_lines;
//                    }
//                }
//                else {
//                    read_by_lines += read_line(buffer + prev_read, line, BUFFER_SIZE);
//                }
//            }
//
//            if (read_by_lines == prev_read) {
//                strcpy(line, buffer + prev_read);
//                chars_left_in_line = (size_t)rcv_len - prev_read;
//            }
//        }
//
//        //printf("Prev: %d\n", prev_read);
//
//        if (encoding_is_chunked && empty_line_encoutered && !zero_chunk_encountered) {
//
//            while (!zero_chunk_encountered && prev_read != rcv_len) {
//                //fprintf(stderr, "JOLO\n");
//
//                // when you need to read chunk size
//                if (still_to_read_in_chunk == 0) {
////                    if (BUFFER_SIZE - chars_left_in_line == 0) {
////                        fatal("Chunk-size line is too long");
////                    }
//
//                    read_in_line = read_line(buffer + prev_read, line + chars_left_in_line, BUFFER_SIZE - chars_left_in_line);
//
//                    chars_left_in_line = 0;
//
//                    // when nothing was read put incomplete line into line
//                    if (read_in_line == 0) {
//                        strcpy(line, buffer + prev_read);
//                        chars_left_in_line = (size_t)rcv_len - prev_read;
//                    }
//                    else {
//                        prev_read += read_in_line;
//
////                        printf("Still to str: %s\n", line);
//
//                        errno = 0;
//
//                        chunk_len = strtoull(line, &end_ptr, 16);
//
//                        if (errno != 0 || (chunk_len == 0 && line == end_ptr)) {
//                            fatal("Unable to read all chunked data");
//                        }
//
//                        still_to_read_in_chunk = chunk_len + 2; // "\r\n"
//
////                        printf("Still to read in chunk: %d\n", still_to_read_in_chunk);
//
//                        if (chunk_len == 0) {
//                            zero_chunk_encountered = true;
//                        }
//                    }
//                }
//                else {
//                    if (rcv_len - prev_read >= still_to_read_in_chunk) {
//                        content_len_res += still_to_read_in_chunk - 2;
//                        prev_read += still_to_read_in_chunk;
//                        still_to_read_in_chunk = 0;
//                    }
//                    else {
//                        content_len_res += rcv_len - prev_read;
//                        still_to_read_in_chunk -= rcv_len - prev_read;
//
//                        // when add whitespace from http syntax to content length
//                        if (still_to_read_in_chunk == 1) {
//                            content_len_res--;
//                        }
//
//                        prev_read = rcv_len;
//                    }
//                }
//            }
//        }
//
////        printf("read from socket: %zd bytes: %s\n", rcv_len, buffer);
//    }
//
//    if (encoding_is_chunked && !zero_chunk_encountered) {
//        fatal("Unable to read all chunked data");
//    }

//    printf("http_len: %" PRIu64 ", my_len: %" PRIu64 "\n", content_len_field, content_len_res);

//    if (status_is_ok) {
//        printf("Dlugosc zasobu: %" PRIu64 "\n", content_len_res);
//    }

    uint64_t content_len_result;

    if (handle_http_response(sock, BUFFER_SIZE, &content_len_result) == 0) {
        printf("Dlugosc zasobu: %" PRIu64 "\n", content_len_result);
    }

    // free
    free(address);
    free_cookies_array(cookies, num_of_cookies);
    free(host);
    free(port);
    free(resource);
    free_header_field_array(header_fields, num_of_http_header_fields);
    free_http_request(http_req);
//    free(http_req_str);

    (void) close(sock);


    return 0;
}