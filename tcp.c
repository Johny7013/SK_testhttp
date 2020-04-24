#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include "err.h"
#include "tcp.h"

// returns sock number, if error then -1
int connect_with_server(char* address, char* port) {
    int sock;
    struct addrinfo addr_hints;
    struct addrinfo *addr_result;

    int err;

    // 'converting' host/port in string to struct addrinfo
    memset(&addr_hints, 0, sizeof(struct addrinfo));
    addr_hints.ai_family = AF_INET; // IPv4
    addr_hints.ai_socktype = SOCK_STREAM;
    addr_hints.ai_protocol = IPPROTO_TCP;
    err = getaddrinfo(address, port, &addr_hints, &addr_result);
    if (err == EAI_SYSTEM) { // system error
        syserr("getaddrinfo: %s", gai_strerror(err));
        return -1;
    }
    else if (err != 0) { // other error (host not found, etc.)
        fatal("getaddrinfo: %s", gai_strerror(err));
        return -1;
    }

    // initialize socket according to getaddrinfo results
    sock = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);
    if (sock < 0) {
        syserr("socket");
        //freeaddrinfo
        return -1;
    }

    // connect socket to the server
    if (connect(sock, addr_result->ai_addr, addr_result->ai_addrlen) < 0) {
        syserr("connect");
        //freeaddrinfo
        return -1;
    }

    freeaddrinfo(addr_result);

    return sock;
}

int write_to_socket(int sock, char* msg) {
    size_t len = strlen(msg);
//    printf("writing to socket: %s\n", http_req_str);
    if (write(sock, msg, len) != len) {
        syserr("partial / failed write");
        return -1;
    }

}

ssize_t read_from_socket(int sock, char* buffer, size_t max_data_to_read) {
    ssize_t rcv_len = read(sock, buffer, max_data_to_read);
    if (rcv_len < 0) {
        syserr("read");
        return -1;
    }

    buffer[rcv_len] = '\0';

    return rcv_len;
}
