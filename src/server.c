// server.c

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "protocol.h"
#include "server.h"
#include "utils.h"

int main(int argc, char** argv)
{
    uint16_t ports[3] = {0};

    if (argc != 4) {
        error("server must receive exactly 3 arguments", 1);
    }

    if (!readPorts(argv, ports)) {
        char msg[52] = {0};
        sprintf(msg, "ports must be between %u and %u (inclusive)", MIN_PORT_NO, MAX_PORT_NO);
        error(msg, 2);
    }

    if (ports[0] == ports[1] || ports[0] == ports[2] || ports[1] == ports[2]) {
        error("port numbers must be unique", 3);
    }

    serve(ports[0], ports[1], ports[2]);

    return EXIT_SUCCESS;
}

void serve(uint16_t engPort, uint16_t maoPort, uint16_t gerPort)
{

}

bool readPorts(char** argv, uint16_t* ports)
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
