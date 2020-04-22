#include <memory.h>
#include <stdbool.h>

#include "http.h"
#include "err.h"

http_header_field make_header_field(const char* header, const char* val) {
    size_t header_len = strlen(header), val_len = strlen(val);

    if (header_len + val_len + NUM_OF_ADDITIONAL_CHARS_HEADER_LINE > MAX_HEADER_FIELD_LEN) {
        fatal("Header field is too lonng. Max length %d", MAX_HEADER_FIELD_LEN);
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