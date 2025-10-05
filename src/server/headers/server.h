#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9001
#define MAX_CONNECTION 5
#define BUFFER_SIZE 1024

void startServer(int NODE_PORT);
// void handle_client(int new_socket);
void connectPeer(int port);
void sendPeer(const char *peer_ip, int peer_port, const char *message);
//struct sockadrr_in server_address;.


#endif
