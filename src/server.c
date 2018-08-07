// server.c

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/poll.h>
#include <sys/time.h>

#include "protocol.h"
#include "server.h"
#include "utils.h"

int socket_fds[3];

int main(int argc, char** argv)
{
    uint16_t ports[3] = {0};

    // validate the arguments
    if (argc != 4) {
        error("server must receive exactly 3 arguments", 1);
    }

    // read the ports into the ports array
    if (!readPorts(argv, ports)) {
        char msg[52] = {0};
        sprintf(msg, "ports must be between %u and %u (inclusive)", MIN_PORT_NO, MAX_PORT_NO);
        error(msg, 1);
    }

    // check that the ports are unique
    if (ports[0] == ports[1] || ports[0] == ports[2] || ports[1] == ports[2]) {
        error("port numbers must be unique", 1);
    }

    // handle some signals so that the sockets can shutdown 
    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    serve(ports);

    return EXIT_SUCCESS;
}

/**
 * Gracefully shutdown the server by closing the sockets.
 * 
 * @param sig The signal sent to the program.
 * */
void handleSignal(int sig)
{
    // close the sockets and exit
    printf("Closing sockets...\n");

    for (int i = 0; i < 3; i++) {
        close(socket_fds[i]);
    }

    exit(0);
}

/**
 * Serves on all three ports.
 * 
 * @param The list of ports to serve on.
 * */
void serve(uint16_t ports[])
{
    int n;
    struct sockaddr_in s_addr[3], c_addr;
    socklen_t c_len = sizeof(c_addr);
    uint8_t buffer[256];
    uint8_t res[RES_PKT_LEN];
    uint16_t reqType;
    
    int optval;

    fd_set rfds;

    // create three sockets for the three ports
    for (int i = 0; i < 3; i++) {

        socket_fds[i] = socket(AF_INET, SOCK_DGRAM, 0);

        if (socket_fds[i] < 0) {
            error("could not create a socket", 2);
        }

        // lets us reuse the port after killing the server.
        optval = 1;
        setsockopt(socket_fds[i], SOL_SOCKET, SO_REUSEADDR,
            (const void *) &optval, sizeof(int));

        // fill out the s_addr struct with information about how we want to serve data
        memset((char *) &s_addr[i], 0, sizeof(s_addr[i]));
        s_addr[i].sin_family = AF_INET;
        s_addr[i].sin_addr.s_addr = INADDR_ANY;
        s_addr[i].sin_port = htons(ports[i]);

        // attempt to bind to the port number
        if (bind(socket_fds[i], (struct sockaddr *) &s_addr[i], sizeof(s_addr[i])) < 0) {
            error("could not bind to socket", 2);
        }

        // print some information and listen
        printf("Listening on port %u for %s requests...\n", ports[i], getLangName(i + 1));

        listen(socket_fds[i], 5);
    
    }

    while (true) {

        int s_sockfd = -1;
        int langCode = -1;

        // reset the rfds struct
        FD_ZERO(&rfds);
        for (int i = 0; i < 3; i++) {
            FD_SET(socket_fds[i], &rfds);
        }

        // perform the select
        int selectResult = select(max(socket_fds, 3) + 1, &rfds, NULL, NULL, NULL);

        // if there was an error selecting
        if (selectResult == -1) {
            error("select failed", 4);
        }

        // iterate through the pollfd values until one is ready to receive data
        // store the socket descriptor and the language code
        for (int i = 0; i < 3; i++) {
            if (FD_ISSET(socket_fds[i], &rfds)) {
                s_sockfd = socket_fds[i];
                langCode = i + 1;
                break;
            }
        }

        // receive data from the client
        n = recvfrom(s_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &c_addr, &c_len);

        // print the time
        printf("%s - ", getCurrentTimeString());

        // if an error occurred during reading the information, print an error
        if (n < 0) {
            printf("network error - packet discarded\n");
            continue;
        }

        // handle the data
        if (!dtReqValid(buffer, n)) {

            printf("invalid request - packet discarded\n");

        } else {

            // print some more information
            reqType = dtReqType(buffer, n);

            printf("%s %s requested - ", getLangName(langCode), getRequestTypeString(reqType));

            // zero the response packet buffer
            memset(res, 0, RES_PKT_LEN);

            // construct the response packet
            size_t b = dtResNow(res, RES_PKT_LEN, reqType, langCode);

            // attempt to sent the response packet
            if (sendto(s_sockfd, res, b, 0, (struct sockaddr *) &c_addr, c_len) < 0) {
                printf("response failed to send\n");
            } else {
                printf("response sent\n");
            }

        }

    }

}

/**
 * Reads the ports from argv and puts them into the ports array.
 * 
 * @param arv The arguments passed into main.
 * @param ports The array to populate with ports.
 * @return True if all ports were valid.
 * */
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

/**
 * Returns the largest value in the array.
 * 
 * @param nums The array to iterate through.
 * @param n The length of the array
 * */
int max(int nums[], int n)
{
    int largest;
    for (int i = 0; i < n; i++) {
        if (i == 0 || nums[i] > largest) {
            largest = nums[i];
        }
    }
    return largest;
}