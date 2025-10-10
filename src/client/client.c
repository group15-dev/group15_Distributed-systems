
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void handle_client(int server_port, const char* username, const char* room) {
    int server_fd;
    struct sockaddr_in servAddr;
    char buffer[BUFFER_SIZE] = {0};
    char message[BUFFER_SIZE];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(server_port);
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(server_fd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server on port %d\n", server_port);

    // Send HELLO message
    snprintf(message, sizeof(message), "HELLO %s %s\n", username, room);
    send(server_fd, message, strlen(message), 0);

    // Receive welcome message
    ssize_t valread = recv(server_fd, buffer, BUFFER_SIZE, 0);
    if (valread > 0) {
        printf("Server: %s", buffer);
    }

    // Chat loop
    printf("Enter messages (type 'quit' to exit):\n");
    while (1) {
        printf("%s@%s> ", username, room);
        fflush(stdout);
        
        if (fgets(message, sizeof(message), stdin) == NULL) {
            break;
        }

        // Remove newline
        message[strcspn(message, "\n")] = 0;

        if (strcmp(message, "quit") == 0) {
            break;
        }

        // Send message to server
        char send_msg[BUFFER_SIZE];
        snprintf(send_msg, sizeof(send_msg), "SEND %s %s\n", room, message);
        send(server_fd, send_msg, strlen(send_msg), 0);

        // Receive response
        memset(buffer, 0, BUFFER_SIZE);
        valread = recv(server_fd, buffer, BUFFER_SIZE, 0);
        if (valread > 0) {
            printf("Server: %s", buffer);
        }
    }

    close(server_fd);
}
