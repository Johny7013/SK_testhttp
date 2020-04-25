// Author: Jan Klinkosz, id number: 394 342

#ifndef SK_MALE_ZADANIE1920_HTTP_REQUEST_H
#define SK_MALE_ZADANIE1920_HTTP_REQUEST_H

#include <stdbool.h>
#include <inttypes.h>

#include "cookies.h"

// known HTTP versions
#define HTTP_VERSION10 "HTTP/1.0"
#define HTTP_VERSION11 "HTTP/1.1"

#define HTTP_OK_STATUS "200 OK"

// request methods
#define GET "GET"
#define HEAD "HEAD"
#define POST "POST"
#define PUT "PUT"
#define OPTIONS "OPTIONS"
#define DELETE "DELETE"
#define CONNECT "CONNECT"
#define TRACE "TRACE"
#define PATCH "PATCH"

// according to RFC7230 min len is 8000
// APACHE Servers limit is 8192
#define MAX_HEADER_FIELD_LEN 8192

#define NUM_OF_ADDITIONAL_CHARS_FIRST_LINE 4 // whitespaces, carriage return,  endl
#define NUM_OF_ADDITIONAL_CHARS_HEADER_LINE 4 // whitespaces, carriage return,  endl

struct HTTP_header_field;

typedef struct HTTP_header_field* http_header_field;

struct HTTP_header_field {
    char *header;
    char *val;
};

// makes header_field form header and val of this header
// allocates memory for header_field, which need to be freed (by free_header_field function)
//
// returns created http_header_field - on success , NULL pointer - on failure
http_header_field make_header_field(const char* header, const char* val);

// copies header_field
// allocates memory for new http_header_field, which need to be freed (by free_header_field function)
//
// returns new allocated header_filed - on success, NULL pointer - on failure
http_header_field copy_header_field(http_header_field header_field);

// frees memory allocated for http_header_field
void free_header_field(http_header_field header_field);

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
                                          size_t* num_of_http_header_fields);

// copies header_fields array
// allocates memory for new header_fields_array and every single header_field, which need to be freed (by free_header_field function)
//
// returns new allocated header_filed - on success, NULL pointer - on failure
http_header_field* copy_header_field_array(http_header_field* header_fields, size_t num_of_header_fields);

// frees memory allocated for header_fields (i.e. array (http_header_fields*) and every
// single header_field, where number of single header_fields to be freed is num_of_fields)
void free_header_field_array(http_header_field* header_fields, size_t num_of_fields);

// returns length of header + length of value of header_field
size_t len_of_content_header_field(http_header_field header_field);

// generates string which is transformation of header_field to line of http_request
// returns syntax valid http request line - on success, NULL pointer - on failure
char* header_field_to_str(http_header_field header_field);

struct HTTP_request;

typedef struct HTTP_request *http_request;

struct HTTP_request {
    char method[8];
    char *resource;
    char version[9];
    http_header_field *header_fields;
    size_t num_of_header_fields;
};

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
                               http_header_field* header_fields, size_t num_of_header_fields);

// generates string which is transformation of http_req to actual http request
// i.e. with all whitespaces and CRLF etc.
//
// returns http request according to correct syntax - on success, NULL pointer - on failure
char* http_request_to_str(http_request http_req);

// frees memory allocated for http_req
void free_http_request(http_request http_req);

// checks if argument status_line is valid http status line with HTTP_OK_STATUS
// checks only known http versions
//
// returns true - if it is valid status line with HTTT_OK_STATUS, false - otherwise
bool is_status_ok(char* status_line);

// sends http request on sock
//
// transforms http_req to str using function http_request_to_string
// and then sends it on sock using write function
//
// returns 0 - on success, -1 - on failure
int send_http_request(int sock, http_request http_req);

// reads http response from socket sock in pieces into buffer with size buffer_size
// sets content_len_ans on length of content part of http response if server responded with status 200
// in case of chunked data it's length of data in all chunked parts
// otherwise it's length of message body
//
// if server responded with other status than 200 then this function prints on standard output
// status line of response
//
// returns 0 - on success, -1 - on failure
int handle_http_response(int sock, size_t buffer_size, uint64_t* content_len);

#endif //SK_MALE_ZADANIE1920_HTTP_REQUEST_H
