#ifndef SERVER_H
#define SERVER_H

bool readPorts(char** argv, uint16_t* ports);
void serve(uint16_t port, uint16_t langCode);
void handleSignal(int sig);

#endif