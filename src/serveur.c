#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "constants.h"
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "serveur.h"

int running = 1;

int main(){
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc == -1){
        fprintf(stderr, "Can not open an IPv4 socket ! Closing now.");
        return 1;
    }

    setup_server(socket_desc);

    listen(socket_desc, 128);

    printf("Yeah ! Socket is opened with descriptor %d\n", socket_desc);

    while(running){
        struct sockaddr client;
        socklen_t length;
        int ack = accept(socket_desc, &client, &length);
        printf("New connection !\n");
        char buffer;
        do{
            read(ack, &buffer, 1);
            printf("%c", buffer);
        }while(buffer != '.');
        printf("\n");
        char *message = "Welcome to you, but I can't talk more.";
        write(ack, message, strlen(message));
        close(ack);
        printf("Closed connection :-( \n");
    }
    return 0;
}

struct sockaddr_in init_server_descriptor() {
    struct sockaddr_in addr;
    memset((char*)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET; // We are talking about IPv4
    addr.sin_port = htons(SERVER_PORT); // See constants.h for value
    struct in_addr listen_ip;
    inet_aton("0.0.0.0", &listen_ip);
    addr.sin_addr = listen_ip; // Listen on 0.0.0.0
    return addr;
}

void setup_server(int socket_desc) {
    enable_reuse_socket(socket_desc);
    bind_server_socket(socket_desc);
}

void bind_server_socket(int socket_desc) {
    struct sockaddr_in addr = init_server_descriptor();
    bind(socket_desc, (const struct sockaddr *) &addr, sizeof(addr)); // Bind the socket to the address
}

void enable_reuse_socket(int socket_desc) {
    int reuse = REUSE_SOCKETS;
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)); // Setup to allow socket reopen
}