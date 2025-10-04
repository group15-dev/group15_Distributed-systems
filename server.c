#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strng.h>

/**
  * create a socket file descriptor
  * 
  * set socket options to reuse address
  *
  * prepare the sockaddr_in structure
  *
  * bind socket to port
  *
  * listen to incomming connection
  *
  * accept new connections
  *
  * receive data from clients
  */

int start_server(){
    int server_fd;
    struct sockaddr_in servAddr;
    int opt = 1;

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0))==0){
        perror("socket failed\n");
        exit(EXIT_FAILURE);
    }
    

    if((setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR &opt, sizeof(opt) )){
        perror("sockopt failed");
        exit(EXIT_FAILURE);
    }

    

    servAddr.sin_family = AF_INET;
    servAddr.s_addr = INADDR_ANY;
    servAddr.htons(PORT);


    if (bind(server_fd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0){
        perror("Bind Failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3)<0){
        perror("Listen Failed");
        exit(EXIT_FAILURE);
    };

    printf("Server listenig on port %d\n", PORT);
     
    if(new_socket = accept(server_fd, (struct sockaddr*)&servAddr, sizeof(servAddr))<0){
        perror("Accept Failed");
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted from client.\n");
    

    ssize_t valread = recv(new_socket, buffer, BUFFER_SIZE, 0);
    if (valread > 0) {
        printf("Client: %s\n", buffer);
    }

    cont char *hello = "ACK";
    send(new_socket, heelo, strlen(hello), 0);
    printf("ACK sent to client.\n");

    close(new_socket);
    close(server_fd);

}


