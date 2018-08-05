#ifndef SERVER_H
#define SERVER_H

// by default, serve on all interfaces
#define SERVER_ADDR 0x0000

bool readPorts(char** argv, uint16_t* ports);
void serve(uint16_t engPort, uint16_t maoPort, uint16_t gerPort);

#endif