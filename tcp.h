// Author: Jan Klinkosz, id number: 394 342

#ifndef SK_MALE_ZADANIE1920_TCP_H
#define SK_MALE_ZADANIE1920_TCP_H

#include <stddef.h>
#include <sys/types.h>

// connects to server with address:port via TCP
//
// returns sock number - on success, -1 - on failure
int connect_with_server(char* address, char* port);

// writes msg to socket sock with write function
//
// returns 0 - on success, -1 - on failure
int write_to_socket(int sock, char* msg);

// reads socket sock into buffer with read function
// reads up to max_data_to_read into buffer ends data read with '\0' char,
// which is also put into buffer.
// Size of buffer needs to be big enough to store max_data_to_read + 1 bytes
//
// returns number of bytes read form socket - on success, -1 - on failure
ssize_t read_from_socket(int sock, char* buffer, size_t max_data_to_read);

#endif //SK_MALE_ZADANIE1920_TCP_H
