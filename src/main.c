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
    if (argc < 2){
        fprintf(stderr, "Usage: %s <port1> [<port2> ...] \n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++){
        int port = atoi(argv[i]);

        if (port <= 0){
           fprintf(stderr, "Invalid port: %s\n", argv[i]);
            continue;
        }
        startServer(port);
    }

    // :wq
    //
    // startServer(port);
    return(0);

}
