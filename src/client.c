// client.c

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "client.h"
#include "protocol.h"
#include "utils.h"

/**
 * Usage: client <time|date> <ip address> <port>
 * */
int main(int argc, char** argv)
{
    uint16_t request_type, port;

    // validate the number of arguments passed in
    if (argc != 4) {
        error("client expects exactly 4 arguments", 1);
    }

    // set the request_type based on the first argument
    if (strcmp(argv[1], "date") == 0) {
        request_type = REQ_DATE;
    } else if (strcmp(argv[1], "time") == 0) {
        request_type = REQ_TIME;
    } else {
        error("first argument should be either \"date\" or \"time\"" ,1);
    }

    // set the port based on the third argument
    port = atoi(argv[3]);
    if (port < MIN_PORT_NO || port > MAX_PORT_NO) {
        char msg[55] = {0};
        sprintf(msg, "the port must be between %u and %u (inclusive)", MIN_PORT_NO, MAX_PORT_NO);
        error(msg, 1);
    }

    // send a request
    request(request_type, argv[2], argv[3]);

    return 0;
}

/**
 * Sends a request to the server.
 * 
 * @param request_type The type of request, either REQ_DATE or REQ_TIME.
 * @param ip_address_string The ip address of the server as a string.
 * @param port The port the server is listening on.
 * */
void request(uint16_t request_type, char* ip_address_string, char* port_string)
{
    
    // the address information of the server
    // struct sockaddr_in server_address;
    socklen_t server_address_len;

    // the socket descriptor of the client
    int client_socket;

    // the buffers to hold the raw request and response packets
    uint8_t req[REQ_PKT_LEN] = {0};
    uint8_t buffer[RES_PKT_LEN] = {0};

    // stores the amount of time for select() to wait before returning
    struct timeval timeout;

    // this is required for select() to work
    fd_set socket_set;

    // this is used to test what is returned by select()
    int select_result;

    // set aside some space for the text from the incoming data to be placed
    char text[RES_TEXT_LEN] = {0};

    // denotes the length of the text received.
    size_t text_len = 0;

    // holds the server address hints
    struct addrinfo hints;
    
    // a pointer to all the addresses returned by getaddrinfo
    struct addrinfo *addresses;

    // a pointer to the current address used as the server address
    struct addrinfo *server_address;

    // setup the hints data structure
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    // get the address info
    if (getaddrinfo(ip_address_string, port_string, &hints, &addresses) != 0) {
        error("bad hostname or ip address", 1);
    }
    
    // iterate over every possible server address returned by getaddrinfo
    // and select the first one that we can connect to
    for (server_address = addresses; server_address != NULL; server_address = server_address->ai_next) {

        // create a socket
        client_socket = socket(server_address->ai_family,
            server_address->ai_socktype, server_address->ai_protocol);

        // if it is no good, get another one
        if (client_socket < 0) {
            continue;
        }

        // attempt to connect to the server, if this works, break
        if (connect(client_socket, server_address->ai_addr,
            server_address->ai_addrlen) == 0) {
            break;
        }

        // close this socket and get another one
        close(client_socket);
    }

    // display an error if we could not connect to the server
    if (server_address == NULL) {
        error("could not connect", 1);
    }

    server_address_len = sizeof(server_address);

    // create the packet
    if (dtReq(req, REQ_PKT_LEN, request_type) == 0) {
        error("could not create packet", 3);
    }

    // set the timeout to be one second
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    // set the socket_set
    FD_ZERO(&socket_set);
    FD_SET(client_socket, &socket_set);

    // wait for the socket to be writable
    select_result = select(client_socket + 1, NULL, &socket_set, NULL, &timeout);

    // print an error if something went wrong while selecting
    if (select_result < 0) {
        error("could not select", 4);
    }

    // print an error if a timeout occurred
    if (select_result == 0) {
        error("select timed out", 4);
    }

    // attempt to send the packet
    if (sendto(client_socket, req, REQ_PKT_LEN, 0, (struct sockaddr *) server_address->ai_addr, server_address->ai_addrlen) < 0) {
        error("could not send packet", 2);
    }

    // set the timeout to be one second, this must be set again because select() modifies the timeout
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    // set the socket_set, this must be set again because select() modifies socket_set
    FD_ZERO(&socket_set);
    FD_SET(client_socket, &socket_set);

    // Wait for the socket to be readable
    select_result = select(client_socket + 1, &socket_set, NULL, NULL, &timeout);

    // print an error if something went wrong while selecting
    if (select_result < 0) {
        error("could not select", 4);
    }

    // print an error if a timeout occurred
    if (select_result == 0) {
        error("select timed out", 4);
    }

    // attempt to receive the response
    if (recvfrom(client_socket, buffer, RES_PKT_LEN, 0, (struct sockaddr *) &server_address, &server_address_len) < 0) {
        error("could not recieve packet", 2);
    }

    // free the memory used by getaddrinfo
    freeaddrinfo(addresses);

    // close the socket
    close(client_socket);

    // extract the text, storing it in text and the length in text_len
    dtResText(buffer, dtPktLength(buffer), text, &text_len);

    // print the other information
    printf("MagicNo:\t0x%04X\n", dtPktMagicNo(buffer, RES_PKT_LEN));
    printf("PacketType:\t%u\n", dtPktType(buffer, RES_PKT_LEN));
    printf("LanguageCode:\t%u\n", dtResLangCode(buffer, RES_PKT_LEN));
    printf("Year:\t\t%u\n", dtResYear(buffer, RES_PKT_LEN));
    printf("Month:\t\t%u\n", dtResMonth(buffer, RES_PKT_LEN));
    printf("Day:\t\t%u\n", dtResDay(buffer, RES_PKT_LEN));
    printf("Hour:\t\t%u\n", dtResHour(buffer, RES_PKT_LEN));
    printf("Minute:\t\t%u\n", dtResMinute(buffer, RES_PKT_LEN));
    printf("Length:\t\t%u\n", dtResLength(buffer, RES_PKT_LEN));

    // print the text response
    printf("Text:\t\t%s\n", text);

}
