// server.h

#ifndef SERVER_H
#define SERVER_H

bool readPorts(char** argv, uint16_t* ports);
void serve(uint16_t ports[]);
void handleSignal(int sig);

#endif