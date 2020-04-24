#ifndef SK_MALE_ZADANIE1920_TCP_H
#define SK_MALE_ZADANIE1920_TCP_H

#include <stddef.h>
#include <sys/types.h>

// returns sock number, if error then -1;
int connect_with_server(char* address, char* port);

int write_to_socket(int sock, char* msg);

ssize_t read_from_socket(int sock, char* buffer, size_t max_data_to_read);

#endif //SK_MALE_ZADANIE1920_TCP_H
