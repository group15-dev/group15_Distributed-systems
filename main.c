#include <stdio.h>

// file set up
//
//----------------
//create node
//----------------
/**
  * client conncetiomn
  * route message btw node
  * maintain session
  * sync message history
  */




int main(){
    putchar('H');
    
    FILE *file = fopen("output.txt", "w");

    if (file != NULL){
        putc('H', file);
    }

    return(0);

}
