// client.h

#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>

int main(int argc, char** argv);
void request(uint16_t reqType, char* ip_addr, uint16_t port);

#endif