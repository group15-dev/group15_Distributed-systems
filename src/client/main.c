#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_port> <username> <room>\n", argv[0]);
        fprintf(stderr, "Example: %s 9001 alice general\n", argv[0]);
        return 1;
    }

    int server_port = atoi(argv[1]);
    const char* username = argv[2];
    const char* room = argv[3];

    if (server_port <= 0) {
        fprintf(stderr, "Invalid port number. Port must be > 0.\n");
        return 1;
    }

    printf("Starting chat client...\n");
    printf("Connecting to server on port %d as user '%s' in room '%s'\n", 
           server_port, username, room);

    handle_client(server_port, username, room);

    return 0;
}
