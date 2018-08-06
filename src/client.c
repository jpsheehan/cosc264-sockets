// client.c

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <fcntl.h>

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
        char msg[55] = {0};
        sprintf(msg, "the port must be between %u and %u (inclusive)", MIN_PORT_NO, MAX_PORT_NO);
        error(msg, 1);
    }

    // send a request
    request(reqType, argv[2], port);

    return 0;
}

/**
 * Sends a request to the server.
 * 
 * @param reqType The type of request, either REQ_DATE or REQ_TIME.
 * @param ip_addr The ip address of the server as a string.
 * @param port The port the server is listening on.
 * */
void request(uint16_t reqType, char* ip_addr, uint16_t port)
{
    struct sockaddr_in s_addr;
    socklen_t s_len = sizeof(s_addr);

    struct pollfd pfd;

    uint8_t req[REQ_PKT_LEN] = {0};
    uint8_t buffer[RES_PKT_LEN] = {0};

    // create the socket
    pfd.fd = socket(AF_INET, SOCK_DGRAM, 0);
    pfd.events = POLLOUT;

    if (pfd.fd < 0) {
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
    if (dtReq(req, REQ_PKT_LEN, reqType) == 0) {
        error("could not create packet", 3);
    }

    // int flags;

    //set socket nonblocking flag
    // if( (flags = fcntl(pfd.fd, F_GETFL, 0)) < 0)
    //     error("error getting flags", 4);
    
    // if(fcntl(pfd.fd, F_SETFL, flags | O_NONBLOCK) < 0)
    //     error("error setting flags", 4);

    if (connect(pfd.fd, (struct sockaddr *) &s_addr, s_len) < 0) {
        error("couldn't connect", 4);
    }

    int pollResult = poll(&pfd, 1, 1000);

    if (pollResult < 0) {
        error("could not poll", 4);
    }

    if (pollResult == 0) {
        error("poll timed out", 4);
    }

    // attempt to send the packet
    if (sendto(pfd.fd, req, REQ_PKT_LEN, 0, (struct sockaddr *) &s_addr, s_len) < 0) {
        error("could not send packet", 2);
    }

    // attempt to receive the response
    if (recvfrom(pfd.fd, buffer, RES_PKT_LEN, 0, (struct sockaddr *) &s_addr, &s_len) < 0) {
        error("could not recieve packet", 2);
    }

    // close the socket
    close(pfd.fd);

    // print the response
    char response[RES_TEXT_LEN] = {0};
    size_t response_len = 0;
    dtResText(buffer, dtPktLength(buffer), response, &response_len);
    printf("%s\n", response);

}
