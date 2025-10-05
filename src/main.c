#include <stdio.h>
#include "headers/server.h"



int main(int argc, char *argv[]){
/**
  * 
  
    putchar('H');
    
    FILE *file = fopen("output.txt", "w");

    if (file != NULL){
        putc('H', file);
    }
 */
    if (argc !=3){
        fprintf(stderr, "Usage: %s <server_port> [<peer_port> ...] \n", argv[0]);
        return 1;
    }

    // for (int i = 1; i < argc; i++){
        int server_port = atoi(argv[1]);
        int peer_port = atoi(argv[2]);

        if (server_port <= 0 || peer_port <= 0){
           fprintf(stderr, "Invalid portnumber. Ports must be > 0.\n");
            return(1);
        }
        startServer(server_port);
        connectPeer(peer_port);
    // }

    // :wq
    //
    // startServer(port);
    return(0);

}
