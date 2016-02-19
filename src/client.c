#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "client.h"
#include "constants.h"

void enable_reuse_socket(int socket_desc);

int main(){
    int socket_desc;
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0)
    {
        printf("Socket creation error\n");
        exit(1);
    }
    struct sockaddr_in addr;
    memset((char*)&addr, 0, sizeof(addr));
    addr.sin_family= AF_INET;
    addr.sin_port=htons(SERVER_PORT);
    struct in_addr listen_ip;
    inet_aton("127.0.0.1", &listen_ip);
    addr.sin_addr = listen_ip;

    enable_reuse_socket(socket_desc);

    printf("Socket is opened with description: %d\n", socket_desc);


    return 0;
}

void enable_reuse_socket(int socket_desc) {
        int reuse= REUSE_SOCKETS;
        setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    }