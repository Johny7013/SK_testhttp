#ifndef SK_MALE_ZADANIE1920_HTTP_REQUEST_H
#define SK_MALE_ZADANIE1920_HTTP_REQUEST_H

#include "cookies.h"

#define HTTP_VERSION "HTTP/1.1"

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

#define MAX_HEADER_FIELD_LEN 8192

#define NUM_OF_ADDITIONAL_CHARS_FIRST_LINE 3
#define NUM_OF_ADDITIONAL_CHARS_HEADER_LINE 3

struct HTTP_header_field;

typedef struct HTTP_header_field *http_header_field;

struct HTTP_header_field {
    char *header;
    char *val;
};

http_header_field make_header_field(const char* header, const char* val);

void free_header_field(http_header_field header_field);

void free_header_field_array(http_header_field* header_fields, size_t num_of_fields);

size_t len_of_content_header_field(http_header_field header_field);

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

//TODO rewrite code to copy values (resource, header_fields)
http_request make_http_request(const char method[], char* resource, const char version[],
                               http_header_field* header_fields, size_t num_of_header_fields);

char* http_request_to_str(http_request http_req);

#endif //SK_MALE_ZADANIE1920_HTTP_REQUEST_H
