#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "constants.h"

int running = 1;

void enable_reuse_socket(int socket_desc);

int main(){
    int socket_desc;
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0)
    {
        printf("Socket creation error\n");
        exit(1);
    }

    enable_reuse_socket(socket_desc);

    struct sockaddr_in addr;
    memset((char*)&addr, 0, sizeof(addr));
    addr.sin_family= AF_INET;
    addr.sin_port=htons(SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* set destination IP number - localhost, 127.0.0.1*/

    printf("Socket is opened with description: %d\n", socket_desc);

    int i = connect(socket_desc, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    if(i < 0)
        printf("Failed to connect to server");

    char message[] = "Hello my faboulous world!";
    write(socket_desc, "Hello\n", 6);

    return 0;
}

void enable_reuse_socket(int socket_desc) {
        int reuse= REUSE_SOCKETS;
        setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    }