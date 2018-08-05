// client.c

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#include "client.h"
#include "protocol.h"
#include "utils.h"

int main(int argc, char** argv)
{
    uint16_t reqType, port;

    if (argc != 4) {
        error("client expects exactly 4 arguments", 1);
    }

    // set the reqType
    if (strcmp(argv[1], "date") == 0) {
        reqType = REQ_DATE;
    } else if (strcmp(argv[1], "time") == 0) {
        reqType = REQ_TIME;
    } else {
        error("first argument should be either \"date\" or \"time\"" ,1);
    }

    // set the port
    port = atoi(argv[3]);
    if (port < MIN_PORT_NO || port > MAX_PORT_NO) {
        error("third argument should be a valid port number", 1);
    }

    request(reqType, argv[2], port);

    return 0;
}

void request(uint16_t reqType, char* ip_addr, uint16_t port)
{
    struct sockaddr_in s_addr;
    int s_len = sizeof(s_addr);
    int c_sockfd;
    uint8_t buffer[REQ_PKT_LEN] = {0};

    // create the socket
    c_sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (c_sockfd < 0) {
        error("could not create socket", 2);
    }

    // setup the server address struct
    memset((char*) &s_addr, 0, s_len);
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_addr, &s_addr.sin_addr) == 0) {
        error("invalid ip address", 1);
    }

    // create the packet
    if (dtReq(buffer, REQ_PKT_LEN, reqType) == 0) {
        error("could not create packet", 3);
    }

    if (sendto(c_sockfd, buffer, REQ_PKT_LEN, 0, (struct sockaddr *) &s_addr, s_len) < 0) {
        error("could not send packet", 2);
    }

    close(c_sockfd);

    printf("packet sent\n");
    dtPktDump(buffer);

}
