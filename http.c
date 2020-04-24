#include <memory.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>
#include <stdio.h>

#include "http.h"
#include "utils.h"
#include "err.h"
#include "tcp.h"

http_header_field make_header_field(const char* header, const char* val) {
    size_t header_len = strlen(header), val_len = strlen(val);

    if (header_len + val_len + NUM_OF_ADDITIONAL_CHARS_HEADER_LINE > MAX_HEADER_FIELD_LEN) {
        fatal("Header field is too long. Max length %d", MAX_HEADER_FIELD_LEN);
        return NULL;
    }

    http_header_field created_header_field = (http_header_field) malloc(sizeof(struct HTTP_header_field));
    char* field_header = (char *) malloc(sizeof(char) * (header_len + 1));
    char* field_val = (char *) malloc(sizeof(char) * (val_len + 1));

    strcpy(field_header, header);
    strcpy(field_val, val);

    created_header_field->header = field_header;
    created_header_field->val = field_val;

    return created_header_field;
}

void free_header_field(http_header_field header_field) {
    free(header_field->header);
    free(header_field->val);
    free(header_field);
}

http_header_field* generate_header_fields(cookie* cookies, size_t num_of_cookies, const char* host,
                                          size_t* num_of_http_header_fields) {
    *num_of_http_header_fields = num_of_cookies + 2; //+2 because of host and connection headers

    http_header_field* header_fields = malloc(sizeof(http_header_field) * (*num_of_http_header_fields));

    if (header_fields == NULL) {
        syserr("Unable to allocate memory");
        goto error_exit;
    }
    else {
        for (size_t i = 0; i < *num_of_http_header_fields; i++) {
            header_fields[i] = NULL;
        }
    }

    header_fields[0] = make_header_field("Host", host);
    header_fields[1] = make_header_field("Connection", "close");

    char* cookie_str;

    for (size_t i = 2; i < *num_of_http_header_fields; i++) {
        cookie_str = cookie_to_str(cookies[i - 2]);

        if (cookie_str == NULL) {
            fatal("Unable to allocate memory");
            goto clean_up;
        }

        header_fields[i] = make_header_field("Cookie", cookie_str);
        free(cookie_str);
    }

    // check if any header_field wasn't generated properly
    for (size_t i = 0; i < *num_of_http_header_fields; i++) {
        if (header_fields[i] == NULL) {
            fatal("Unable to allocate memory");
            goto clean_up;
        }
    }

    return header_fields;

clean_up:
    free_header_field_array(header_fields, *num_of_http_header_fields);

error_exit:
    return NULL;
}

void free_header_field_array(http_header_field* header_fields, size_t num_of_fields) {
    for (size_t i = 0; i < num_of_fields; i++) {
        free_header_field(header_fields[i]);
    }

    free(header_fields);
}

size_t len_of_content_header_field(http_header_field header_field) {
    return strlen(header_field->header) + strlen(header_field->val);
}

char* header_field_to_str(http_header_field header_field) {
    char* str = (char *) malloc(
            sizeof(char) * (len_of_content_header_field(header_field) + NUM_OF_ADDITIONAL_CHARS_HEADER_LINE + 1));

    strcpy(str, header_field->header);
    strcat(str, ": ");
    strcat(str, header_field->val);
    strcat(str, "\r\n");

    return str;
}

http_request make_http_request(const char method[], char* resource, const char version[],
                               http_header_field* header_fields, size_t num_of_header_fields) {
    http_request created_http_request = (http_request) malloc(sizeof(struct HTTP_request));

    strcpy(created_http_request->method, method);
    created_http_request->resource = resource;
    strcpy(created_http_request->version, version);
    created_http_request->header_fields = header_fields;
    created_http_request->num_of_header_fields = num_of_header_fields;

    return created_http_request;
}

char* http_request_to_str(http_request http_req) {
    size_t method_len = strlen(http_req->method), res_len = strlen(http_req->resource),
           ver_len = strlen(http_req->version), header_fields_len = 0;

    for (size_t i = 0; i < http_req->num_of_header_fields; i++) {
        header_fields_len += len_of_content_header_field(http_req->header_fields[i]);
    }

    size_t str_len = method_len + res_len + ver_len + header_fields_len + NUM_OF_ADDITIONAL_CHARS_FIRST_LINE
                     + NUM_OF_ADDITIONAL_CHARS_HEADER_LINE * http_req->num_of_header_fields + 3; //for CRLF and \0

    bool addSlash =  (res_len == 0 || http_req->resource[0] != '/');

    if (addSlash) {
        str_len++;
    }

    char* str = malloc(sizeof(char) * str_len);

    strcpy(str, http_req->method);
    strcat(str, " ");

    if (addSlash) {
        strcat(str, "/");
    }

    strcat(str, http_req->resource);
    strcat(str, " ");
    strcat(str, http_req->version);
    strcat(str, "\r\n");

    for (size_t i = 0; i < http_req->num_of_header_fields; i++) {
        char* header_field_str = header_field_to_str(http_req->header_fields[i]);

        strcat(str, header_field_str);

        free(header_field_str);
    }
    strcat(str, "\r\n");

    return str;
}

void free_http_request(http_request http_req) {
    free(http_req);
}

bool is_status_ok(char* status_line) {
    size_t ok_status_len = strlen(HTTP_OK_STATUS);
    char* ok_status_line_prefix;
    bool res;

    if (starts_with_prefix(HTTP_VERSION10, status_line)) {
        ok_status_line_prefix = compose_strings(HTTP_VERSION10, HTTP_OK_STATUS, " ");
    }
    else if (starts_with_prefix(HTTP_VERSION11, status_line)) {
        ok_status_line_prefix = compose_strings(HTTP_VERSION11, HTTP_OK_STATUS, " ");
    }
    else {
        fatal("Unknown HTTP version in response");
        return false;
    }

    res = starts_with_prefix(ok_status_line_prefix, status_line);

    free(ok_status_line_prefix);

    return res;
}

int send_http_request(int sock, http_request http_req) {
    char* http_req_str = http_request_to_str(http_req);

    int res = write_to_socket(sock, http_req_str);

    if (res < 0) {
        fatal("Unable to send http request");
    }

    free(http_req_str);

    return res;
}

int handle_http_response(int sock, size_t buffer_size, uint64_t* content_len_ans) {
    char buffer[buffer_size];
    char line[buffer_size];
    size_t chars_left_in_line = 0, still_to_read_in_chunk = 0;
    ssize_t rcv_len = 1, read_by_lines, prev_read, read_in_line;
    uint64_t content_len_field = 0, content_len_res = 0, chunk_len = 0;
    bool empty_line_encoutered = false, status_line_encountered = false, status_is_ok = false;
    bool encoding_is_chunked = false, zero_chunk_encountered = false;
    char* end_ptr;

    memset(buffer, 0, sizeof(buffer));

    while (rcv_len > 0) {

        //read
        rcv_len = read_from_socket(sock, buffer, buffer_size - 1);

        if (rcv_len < 0) {
            syserr("JOLO");
        }

        if(empty_line_encoutered && !encoding_is_chunked) {
            content_len_res += rcv_len;
        }

        prev_read = 0;

        if (!empty_line_encoutered) {

            read_by_lines = read_line(buffer, line + chars_left_in_line, buffer_size  - chars_left_in_line);

            chars_left_in_line = 0;

            while (read_by_lines - prev_read > 0 && !empty_line_encoutered) {

                printf("Line: %s", line);

                if (!status_line_encountered) {
                    status_is_ok = is_status_ok(line);
                    if (!status_is_ok) {
                        printf("%s", line);
                    }

                    status_line_encountered = true;
                }

                if (status_is_ok) {
                    if (starts_with_prefix("Content-Length:", line)) {
                        char* con_len;
                        char* val;
                        bisect_string(line, &con_len, &val, ':');
                        content_len_field = strtoull(val, NULL, 10);
                    }

                    if (line_sets_cookie(line)) {
                        cookie retrieved_cookie = retrieve_cookie_from_set_cookie(line);
                        print_cookie(retrieved_cookie);
                        free_cookie(retrieved_cookie);
                    }

                    if (line_sets_transfer_encoding_chunked(line)) {
                        encoding_is_chunked = true;
                    }
                }

                prev_read = read_by_lines; // set how many characters you already read

                if (is_empty_line(line)) {
                    empty_line_encoutered = true;

                    if (!encoding_is_chunked) {
                        content_len_res += rcv_len - read_by_lines;
                    }
                }
                else {
                    read_by_lines += read_line(buffer + prev_read, line, buffer_size);
                }
            }

            if (read_by_lines == prev_read) {
                strcpy(line, buffer + prev_read);
                chars_left_in_line = (size_t)rcv_len - prev_read;
            }
        }

        //printf("Prev: %d\n", prev_read);

        if (encoding_is_chunked && empty_line_encoutered && !zero_chunk_encountered) {

            while (!zero_chunk_encountered && prev_read != rcv_len) {
                //fprintf(stderr, "JOLO\n");

                // when you need to read chunk size
                if (still_to_read_in_chunk == 0) {
//                    if (BUFFER_SIZE - chars_left_in_line == 0) {
//                        fatal("Chunk-size line is too long");
//                    }

                    read_in_line = read_line(buffer + prev_read, line + chars_left_in_line, buffer_size - chars_left_in_line);

                    chars_left_in_line = 0;

                    // when nothing was read put incomplete line into line
                    if (read_in_line == 0) {
                        strcpy(line, buffer + prev_read);
                        chars_left_in_line = (size_t)rcv_len - prev_read;
                    }
                    else {
                        prev_read += read_in_line;

                        errno = 0;

                        chunk_len = strtoull(line, &end_ptr, 16);

                        if (errno != 0 || (chunk_len == 0 && line == end_ptr)) {
                            fatal("Unable to read all chunked data");
                        }

                        still_to_read_in_chunk = chunk_len + 2; // "\r\n"

//                        printf("Still to read in chunk: %d\n", still_to_read_in_chunk);

                        if (chunk_len == 0) {
                            zero_chunk_encountered = true;
                        }
                    }
                }
                else {
                    if (rcv_len - prev_read >= still_to_read_in_chunk) {
                        content_len_res += still_to_read_in_chunk - 2;
                        prev_read += still_to_read_in_chunk;
                        still_to_read_in_chunk = 0;
                    }
                    else {
                        content_len_res += rcv_len - prev_read;
                        still_to_read_in_chunk -= rcv_len - prev_read;

                        // when add whitespace from http syntax to content length
                        if (still_to_read_in_chunk == 1) {
                            content_len_res--;
                        }

                        prev_read = rcv_len;
                    }
                }
            }
        }

//        printf("read from socket: %zd bytes: %s\n", rcv_len, buffer);
    }

    if (encoding_is_chunked && !zero_chunk_encountered) {
        fatal("Unable to read all chunked data");
    }

    // set answer
    *content_len_ans = content_len_res;

    return 0;
}