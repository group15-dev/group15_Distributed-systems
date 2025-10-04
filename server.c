#include "server.h"


int start_server(){
 // CREATE SOCKET
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servAddr;

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0))==0){
        perror("socket failed\n");
        exit(EXIT_FAILURE);
    }

    servAddr.sin_family = AF_INET;
    servAddr.htons(PORT);

    bind(server_fd, (struct sockaddr*)&servAddr, sizeof(servAddr));

    listen(server_fd);

}


void handle_client(int new_socket){
  // CREATE CLIENT
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servAddr;

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0))==0){
        perror("socket failed\n");
    exit(EXIT_FAILURE);
    }

    servAddr.sin_family = AF_INET;
    servAddr.htons(PORT);

    int connectStatus = connect(server_fd, (struct sockaddr*)&servAddr, sizeof(servAddr));
    
    if (connectStatus == -1){
        perror("Error...\n");
        exit(EXIT_FAILURE);
    }
    else{
        char strData[255];

        recv(server_)fd, strData, sizeof(strData),0);
        printf("Message: %s\n",strData);

    }
    
}

