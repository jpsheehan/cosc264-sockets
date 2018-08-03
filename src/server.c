// server.c

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "protocol.h"
#include "server.h"

int main(int argc, char** argv)
{
    if (argc != 4) {
        fprintf(stderr, "server must receive exactly 3 arguments\n");
        return 1;
    }

    int ports[3] = {0};
    if (!readPorts(argv, ports)) {
        fprintf(stderr, "ports must be between %d and %d (inclusive)\n", MIN_PORT_NO, MAX_PORT_NO);
        return 2;
    }

    

    return EXIT_SUCCESS;
}

bool readPorts(char** argv, int* ports)
{
    for (int i = 0; i < 3; i++) {
        ports[i] = atoi(argv[i+1]);
        if (ports[i] < MIN_PORT_NO ||
            ports[i] > MAX_PORT_NO) {
            return false;
        }
    }
    return true;
}