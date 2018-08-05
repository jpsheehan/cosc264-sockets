// server.c

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
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
        error(msg, 1);
    }

    if (ports[0] == ports[1] || ports[0] == ports[2] || ports[1] == ports[2]) {
        error("port numbers must be unique", 1);
    }

    serve(ports[0], ports[1], ports[2]);

    return EXIT_SUCCESS;
}

void serve(uint16_t engPort, uint16_t maoPort, uint16_t gerPort)
{
    int s_sockfd, n; // c_sockfd, n;
    struct sockaddr_in s_addr, c_addr;
    socklen_t c_len;
    uint8_t buffer[256];
    uint8_t res[RES_PKT_LEN];
    uint16_t reqType;
    int optval;

    s_sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (s_sockfd < 0) {
        error("could not create a socket", 2);
    }

    // let us reuse the port after killing the server.
    optval = 1;
    setsockopt(s_sockfd, SOL_SOCKET, SO_REUSEADDR,
        (const void *) &optval, sizeof(int));

    memset((char *) &s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_port = htons(engPort);

    if (bind(s_sockfd, (struct sockaddr *) &s_addr, sizeof(s_addr)) < 0) {
        error("could not bind to socket", 2);
    }

    printf("Listening on port %u...\n", engPort);

    listen(s_sockfd, 5);

    c_len = sizeof(c_addr);

    while (true) {

        // c_sockfd = accept(s_sockfd, (struct sockaddr *) &c_addr, &c_len);

        // if (c_sockfd < 0) {
        //     error("could not accept connection", 2);
        // }

        // memset(buffer, 0, sizeof(buffer));

        // n = read(c_sockfd, buffer, sizeof(buffer) - 1);
        // if (n < 0) {
        //     error("could not read from socket", 2);
        // }

        n = recvfrom(s_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &c_addr, &c_len);

        if (n < 0) {
            error("could not recvfrom", 2);
        }

        // handle the data
        if (!dtReqValid(buffer, n)) {

            printf("invalid packet\n");
            dtPktDump(buffer);

        } else {

            printf("valid packet\n");
            dtPktDump(buffer);

            reqType = dtReqType(buffer, n);

            memset(res, 0, RES_PKT_LEN);

            size_t b = dtRes(res, RES_PKT_LEN, reqType, LANG_ENG, 2018, 12, 31, 23, 59);

            // write(c_sockfd, res, b);
            n = sendto(s_sockfd, res, b, 0, (struct sockaddr *) &c_addr, c_len);

            if (n < 0) {
                error("error in sendto", 2);
            }

        }

        // close(c_sockfd);

    }

    close(s_sockfd);

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
