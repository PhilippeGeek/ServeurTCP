//
// Created by tmoutier on 19/02/16.
//
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include "client.h"

int main(){
    int socket_desc;
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0)
    {
        printf("Socket creation error\n");
        exit(1);
    }
    printf("Socket is opened with description: %d\n", socket_desc);
    return 0;
}