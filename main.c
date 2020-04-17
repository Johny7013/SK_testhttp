#include <stdio.h>

#include "err.h"

int main(int argc, char* argv[]) {

    if (argc != 4) {
        fatal("Usage: testhttp_raw address:port cookies tested_http_address");
    }

    return 0;
}