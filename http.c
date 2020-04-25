#include <memory.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>
#include <stdio.h>

#include "http.h"
#include "utils.h"
#include "err.h"
#include "tcp.h"


// makes http_header_field form header and val of this header
// allocates memory for http_header_field, which need to be freed (by free_header_field function)
// stores own copies of header and val
//
// returns created http_header_field - on success , NULL pointer - on failure
http_header_field make_header_field(const char* header, const char* val) {
    size_t header_len = strlen(header), val_len = strlen(val);

    if (header_len + val_len + NUM_OF_ADDITIONAL_CHARS_HEADER_LINE > MAX_HEADER_FIELD_LEN) {
        fatal("Header field is too long. Max length %d", MAX_HEADER_FIELD_LEN);
        return NULL;
    }

    http_header_field created_header_field = (http_header_field) malloc(sizeof(struct HTTP_header_field));
    char* field_header = (char *) malloc(sizeof(char) * (header_len + 1));
    char* field_val = (char *) malloc(sizeof(char) * (val_len + 1));

    if (created_header_field == NULL || field_header == NULL || field_val == NULL) {
        memerr_alloc();
        free(created_header_field);
        free(field_header);
        free(field_val);
        return NULL;
    }

    strcpy(field_header, header);
    strcpy(field_val, val);

    created_header_field->header = field_header;
    created_header_field->val = field_val;

    return created_header_field;
}

// copies header_field
// allocates memory for new http_header_field, which need to be freed (by free_header_field function)
//
// returns new allocated header_filed - on success, NULL pointer - on failure
http_header_field copy_header_field(http_header_field header_field) {
    return make_header_field(header_field->header, header_field->val);
}

// frees memory allocated for http_header_field
void free_header_field(http_header_field header_field) {
    free(header_field->header);
    free(header_field->val);
    free(header_field);
}

// generates array of header_fields that desired http request need to consists of
// it's dedicated to this specific task, generates cookies headers
// connection: close header and Host header,
//
// sets num_of_http_header_fields according to number of generated http_header_fields,
//
// allocates memory for http_header_field* and all single header_fileds,
// which need to be freed (by free_header_field_array function)
//
// returns http_header_fields* which consists of generated http_header_fields - on success,
//         NULL pointer - on failure
http_header_field* generate_header_fields(cookie* cookies, size_t num_of_cookies, const char* host,
                                          size_t* num_of_http_header_fields) {
    *num_of_http_header_fields = num_of_cookies + 2; //+2 because of host and connection headers
    size_t allocated_header_fields = 0;

    http_header_field* header_fields = malloc(sizeof(http_header_field) * (*num_of_http_header_fields));

    if (header_fields == NULL) {
        memerr_alloc();
        goto error_exit;
    }

    header_fields[0] = make_header_field("Host", host);

    if (header_fields[0] == NULL) {
        memerr_alloc();
        goto clean_up;
    }

    allocated_header_fields++;

    header_fields[1] = make_header_field("Connection", "close");

    if (header_fields[1] == NULL) {
        memerr_alloc();
        goto clean_up;
    }

    allocated_header_fields++;

    char* cookie_str;

    for (size_t i = 2; i < *num_of_http_header_fields; i++) {
        cookie_str = cookie_to_str(cookies[i - 2]);

        if (cookie_str == NULL) {
            memerr_alloc();
            goto clean_up;
        }

        header_fields[i] = make_header_field("Cookie", cookie_str);

        free(cookie_str);

        if (header_fields[i] == NULL) {
            memerr_alloc();
            goto clean_up;
        }

        allocated_header_fields++;
    }

    return header_fields;

clean_up:
    free_header_field_array(header_fields, allocated_header_fields);

error_exit:
    return NULL;
}

// copies header_fields array
// allocates memory for new header_fields_array and every single header_field, which need to be freed (by free_header_field function)
//
// returns new allocated header_filed - on success, NULL pointer - on failure
http_header_field* copy_header_field_array(http_header_field* header_fields, size_t num_of_header_fields) {
    http_header_field* copied_header_fields = (http_header_field*) malloc(sizeof(http_header_field) * num_of_header_fields);
    size_t num_of_copied_header_fields = 0;

    if (copied_header_fields == NULL) {
        memerr_alloc();
        return NULL;
    }

    for (size_t i = 0; i < num_of_header_fields; i++) {
        copied_header_fields[i] = copy_header_field(header_fields[i]);

        if (copied_header_fields[i] == NULL) {
            memerr_alloc();
            free_header_field_array(copied_header_fields, num_of_copied_header_fields);
            return NULL;
        }

        num_of_copied_header_fields++;
    }

    return copied_header_fields;
}

// frees memory allocated for header_fields (i.e. array (http_header_fields) and every
// single header_field, where number of single header_fields to be freed is num_of_fields)
void free_header_field_array(http_header_field* header_fields, size_t num_of_fields) {
    for (size_t i = 0; i < num_of_fields; i++) {
        free_header_field(header_fields[i]);
    }

    free(header_fields);
}

// returns length of header + length of value of header_field
size_t len_of_content_header_field(http_header_field header_field) {
    return strlen(header_field->header) + strlen(header_field->val);
}

// generates string which is transformation of header_field to line of http_request
//
// returns http request line according to correct syntax - on success, NULL pointer - on failure
char* header_field_to_str(http_header_field header_field) {
    char* str = (char *) malloc(
            sizeof(char) * (len_of_content_header_field(header_field) + NUM_OF_ADDITIONAL_CHARS_HEADER_LINE + 1));

    if (str == NULL) {
        memerr_alloc();
        return NULL;
    }

    strcpy(str, header_field->header);
    strcat(str, ": ");
    strcat(str, header_field->val);
    strcat(str, "\r\n");

    return str;
}

// makes http request
// stores own copies of data passed
//
// method - http request method
// resource - http request resource
// version - http version
// header_fields - array of header_fileds included in http request
// num_of_header_fields - numbero of header_fields passed in header_fields argument
//
// returns http_request consists of data passed - on success, NULL pointer - on failure
http_request make_http_request(const char method[], char* resource, const char version[],
                               http_header_field* header_fields, size_t num_of_header_fields) {
    size_t resource_len = strlen(resource);

    http_request created_http_request = (http_request) malloc(sizeof(struct HTTP_request));
    char* copied_resource = (char*) malloc(sizeof(char) * (resource_len + 1));
    http_header_field* copied_header_fields = copy_header_field_array(header_fields, num_of_header_fields);

    if (created_http_request == NULL || resource == NULL) {
        memerr_alloc();
        return NULL;
    }
    else if (copied_header_fields == NULL) {
        return NULL;
    }

    strcpy(copied_resource, resource);

    strcpy(created_http_request->method, method);
    created_http_request->resource = copied_resource;
    strcpy(created_http_request->version, version);
    created_http_request->header_fields = copied_header_fields;
    created_http_request->num_of_header_fields = num_of_header_fields;

    return created_http_request;
}

// generates string which is transformation of http_req to actual http request
// i.e. with all whitespaces and CRLF etc.
//
// returns http request according to correct syntax - on success, NULL pointer - on failure
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

    if (str == NULL) {
        memerr_alloc();
        return NULL;
    }

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

        if (header_field_str == NULL) {
            return NULL;
        }

        strcat(str, header_field_str);

        free(header_field_str);
    }
    strcat(str, "\r\n");

    return str;
}

// frees memory allocated for http_req
void free_http_request(http_request http_req) {
    free(http_req->resource);
    free_header_field_array(http_req->header_fields, http_req->num_of_header_fields);
    free(http_req);
}

// checks if argument status_line is valid http status line with HTTP_OK_STATUS
// checks only known http versions
//
// returns true - if it is valid status line with HTTT_OK_STATUS, false - otherwise
bool is_status_ok(char* status_line) {
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

// sends http request on sock
//
// transforms http_req to str using function http_request_to_string
// and then sends it on sock using write function
//
// returns 0 - on success, -1 - on failure
int send_http_request(int sock, http_request http_req) {
    char* http_req_str = http_request_to_str(http_req);

    int res = write_to_socket(sock, http_req_str);

    if (res < 0) {
        fatal("Unable to send http request");
        res = -1;
    }

    free(http_req_str);

    return res;
}

// reads http response from socket sock in pieces into buffer with size buffer_size
// sets content_len_ans on length of content part of http response if server responded with status 200
// in case of chunked data it's length of data in all chunked parts
// otherwise it's length of message body
//
// if server responded with other status than 200 then this function prints on standard output
// status line of response
//
// returns 0 - on success, -1 - on failure
int handle_http_response(int sock, size_t buffer_size, uint64_t* content_len_ans) {
    char buffer[buffer_size];
    char line[buffer_size];
    size_t chars_left_in_line = 0, still_to_read_in_chunk = 0;
    ssize_t rcv_len = 1, read_by_lines, prev_read, read_in_line;
    uint64_t content_len_field = 0, chunk_len = 0;
    uint64_t content_len_res = 0; // local variable, at the end content_len_ans is set on value of this variable
    bool empty_line_encountered = false, status_line_encountered = false, status_is_ok = false;
    bool encoding_is_chunked = false, zero_chunk_encountered = false;
    char* end_ptr;

    memset(buffer, 0, sizeof(buffer));

    while (rcv_len > 0) {

        //read
        rcv_len = read_from_socket(sock, buffer, buffer_size - 1);

        if (rcv_len < 0) {
            return -1;
        }

        // if empty line encountered then all data read from socket
        // is msg body so if encoding is not chunked then add this
        // data length into result (i.e. length of content)
        if(empty_line_encountered && !encoding_is_chunked) {
            content_len_res += rcv_len;
        }

        // bytes data already read from buffer (since last read form socket)
        prev_read = 0;

        if (!empty_line_encountered) {
            errno = 0;
            // read line of htpp resonse from buffer into line
            // read_by_lines - data read from buffer by lines (since last read from socket)
            read_by_lines = read_line(buffer, line + chars_left_in_line, buffer_size  - chars_left_in_line);

            // if found line longer than MAX_HEADER_FIELD_LEN
            if (errno == 2) {
                return -1;
            }

            chars_left_in_line = 0;

            // while it was still sth to read by lines form buffer
            // add empty line still not encountered parse status lines
            // and header lines of http response
            while (read_by_lines - prev_read > 0 && !empty_line_encountered) {

                //TODO remove
                printf("Line: %s", line);

                if (!status_line_encountered) {
                    status_is_ok = is_status_ok(line);
                    if (!status_is_ok) {
                        printf("%s", line);
                    }

                    status_line_encountered = true;
                }

                if (status_is_ok) {
                    //TODO remove
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
                    empty_line_encountered = true;

                    // if encoding isn't chunked then what wasn't read
                    // already is part of msg body and should be added to content_len_res
                    if (!encoding_is_chunked) {
                        content_len_res += rcv_len - read_by_lines;
                    }
                }
                else {
                    errno = 0;

                    // read another line
                    read_by_lines += read_line(buffer + prev_read, line, buffer_size);

                    // if found line longer than MAX_HEADER_FIELD_LEN
                    if (errno == 2) {
                        return -1;
                    }
                }
            }

            // if it still left some data in buffer which wasn't processed
            // then copy it into line
            if (read_by_lines == prev_read && !empty_line_encountered) {
                strcpy(line, buffer + prev_read);
                chars_left_in_line = (size_t)rcv_len - prev_read;
            }
        }

        // when you need to read chunked data
        if (encoding_is_chunked && empty_line_encountered && !zero_chunk_encountered) {

            // while there is still some chunks to read
            // and not all data read from socket was processed
            while (!zero_chunk_encountered && prev_read != rcv_len) {
                // when you need to read chunk size
                if (still_to_read_in_chunk == 0) {
                    errno = 0;

                    read_in_line = read_line(buffer + prev_read, line + chars_left_in_line, buffer_size - chars_left_in_line);

                    // if found line longer than MAX_HEADER_FIELD_LEN
                    if (errno == 2) {
                        return -1;
                    }

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
                            return -1;
                        }

                        still_to_read_in_chunk = chunk_len + 2; // +2 because of "\r\n"

//                        printf("Still to read in chunk: %d\n", still_to_read_in_chunk);

                        if (chunk_len == 0) {
                            zero_chunk_encountered = true;
                        }
                    }
                }
                else {
                    if (rcv_len - prev_read >= still_to_read_in_chunk) {
                        content_len_res += still_to_read_in_chunk - 2; // -2 because of "\r\n"
                        prev_read += still_to_read_in_chunk;
                        still_to_read_in_chunk = 0;
                    }
                    else {
                        content_len_res += rcv_len - prev_read;
                        still_to_read_in_chunk -= rcv_len - prev_read;

                        // when added whitespace from http syntax to content length
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
        return -1;
    }

    //TODO remove
    if (!encoding_is_chunked && content_len_res != content_len_field) {
        fatal("NOT CONFORMANT LEN FIELD");
        exit(EXIT_FAILURE);
    }

    // set answer
    *content_len_ans = content_len_res;

    return 0;
}