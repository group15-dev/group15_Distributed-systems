#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080;
#define MAX_CONNECTION 5;
#define BUFFER_SIZE 1024;

int start_server();
void handle_client(int new_socket);

struct sockadrr_in server_address;


#endif
