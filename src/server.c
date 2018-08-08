// server.c

#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "protocol.h"
#include "server.h"
#include "utils.h"

// the socket descriptors
// these must be global in order to safely close them on SIGINT
int socket_fds[3];

/**
 * Usage: server <english port> <te reo maori port> <german port>
 * */
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

    // handle some signals so that the sockets can shutdown gracefully
    signal(SIGINT, handleSignal);

    // serve on the specified ports
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
    printf("Closing sockets...\n");

    // close the sockets one at a time
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

    // holds the server address information
    struct sockaddr_in server_addr[3];

    // create three sockets for the three ports
    for (int i = 0; i < 3; i++) {

        // required for setsockopt(), set it to 1 to allow us to reuse local addresses
        int option_value = 1;

        socket_fds[i] = socket(AF_INET, SOCK_DGRAM, 0);

        if (socket_fds[i] < 0) {
            error("could not create a socket", 2);
        }

        // lets us reuse the port after killing the server.
        setsockopt(socket_fds[i], SOL_SOCKET, SO_REUSEADDR,
            (const void *) &option_value, sizeof(int));

        // fill out the s_addr struct with information about how we want to serve data
        memset((char *) &server_addr[i], 0, sizeof(server_addr[i]));
        server_addr[i].sin_family = AF_INET;
        server_addr[i].sin_addr.s_addr = INADDR_ANY;
        server_addr[i].sin_port = htons(ports[i]);

        // attempt to bind to the port number
        if (bind(socket_fds[i], (struct sockaddr *) &server_addr[i], sizeof(server_addr[i])) < 0) {
            error("could not bind to socket", 2);
        }

        // print some information and listen
        printf("Listening on port %u for %s requests...\n", ports[i], getLangName(i + 1));

        listen(socket_fds[i], 5);
    
    }

    // loop forever
    while (true) {

        // holds the client address information
        struct sockaddr_in client_addr;

        // the length of the client address data struct
        socklen_t client_addr_len = sizeof(client_addr);

        // holds the IP address of the client
        char client_ip_address_string[INET_ADDRSTRLEN];

        // the active socket that received the request
        // used to determine the port number and to send a response
        int active_socket_fd = -1;

        // the type of request we are handling, read from the request packet
        uint16_t request_type = 0;

        // the language the user requested the response in
        // this is based on the port they connect to
        uint16_t language_code = 0;

        // holds the number of bytes received by the server for a request
        int bytes_received;

        // the buffer to place the received data
        uint8_t buffer[256];

        // the buffer to hold the response data
        uint8_t response[RES_PKT_LEN];

        // holds information on which sockets to wait for while selecting
        fd_set socket_set;

        // reset the socket_set struct
        FD_ZERO(&socket_set);
        for (int i = 0; i < 3; i++) {
            FD_SET(socket_fds[i], &socket_set);
        }

        // perform the select
        int selectResult = select(max(socket_fds, 3) + 1, &socket_set, NULL, NULL, NULL);

        // if there was an error selecting
        if (selectResult == -1) {
            error("select failed", 4);
        }

        // iterate through the pollfd values until one is ready to receive data
        // store the socket descriptor and the language code
        for (int i = 0; i < 3; i++) {

            if (FD_ISSET(socket_fds[i], &socket_set)) {

                active_socket_fd = socket_fds[i];
                language_code = i + 1;

                // break when we find a readable socket descriptor
                // of course this means that English will have priority over 
                // Maori will have priority over German
                break;
            }
        }

        // receive data from the client
        bytes_received = recvfrom(active_socket_fd, buffer, sizeof(buffer), 0,
            (struct sockaddr *) &client_addr, &client_addr_len);

        // get the IP address of the client as a string
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip_address_string, INET_ADDRSTRLEN);

        // print the date, time and the ip address of the client
        printCurrentDateTimeString();
        printf(" - %s - ", client_ip_address_string);

        // if an error occurred during reading the information, print an error
        if (bytes_received < 0) {
            printf("network error - packet discarded\n");
            continue;
        }

        // handle the data
        if (!dtReqValid(buffer, bytes_received)) {

            printf("invalid request - packet discarded\n");

        } else {

            // print some more information
            request_type = dtReqType(buffer, bytes_received);

            printf("%s %s requested - ", getLangName(language_code), getRequestTypeString(request_type));

            // zero the response packet buffer
            memset(response, 0, RES_PKT_LEN);

            // construct the response packet
            size_t b = dtResNow(response, RES_PKT_LEN, request_type, language_code);

            // attempt to sent the response packet
            if (sendto(active_socket_fd, response, b, 0, (struct sockaddr *) &client_addr, client_addr_len) < 0) {
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
