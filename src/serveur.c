#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "constants.h"
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <time.h>
#include "serveur.h"

void handle_new_connection(int socket_desc) ;

int main(int argc, char* argv[]){
    int port;
    switch(argc){
        case 1: printf("No port number defined: Your port number will be 3456\n");
                port=3456;
                break;

        case 2: printf("Your port number is %s\n", argv[1]);
                port=atoi(argv[1]);
                break;

        default: printf("Too many arguments, try again with only one\n");
                 return 1;
    }

    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc == -1){
        fprintf(stderr, "Can not open an IPv4 socket ! Closing now.");
        return 1;
    }

    setup_server(socket_desc, port);

    listen(socket_desc, 128);

    printf("Yeah ! Socket is opened with descriptor %d\n", socket_desc);

    handle_new_connection(socket_desc);
    close(socket_desc);
    return 0;
}

struct sockaddr_in init_server_descriptor(int port) {
    struct sockaddr_in addr;
    memset((char*)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET; // We are talking about IPv4
    addr.sin_port = htons((uint16_t) port); // See constants.h for value
    struct in_addr listen_ip;
    inet_aton("0.0.0.0", &listen_ip);
    addr.sin_addr = listen_ip; // Listen on 0.0.0.0
    return addr;
}

void setup_server(int socket_desc, int port) {
    enable_reuse_socket(socket_desc);
    bind_server_socket(socket_desc, port);
}

void bind_server_socket(int socket_desc, int port) {
    struct sockaddr_in addr = init_server_descriptor(port);
    bind(socket_desc, (const struct sockaddr *) &addr, sizeof(addr)); // Bind the socket to the address
}

void enable_reuse_socket(int socket_desc) {
    int reuse = REUSE_SOCKETS;
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)); // Setup to allow socket reopen
}

fd_set opened_connections;

void handle_new_connection(int socket_desc) {
    struct sockaddr client;
    socklen_t length;
    int ack = accept(socket_desc, &client, &length);
    if(ack<0){
        handle_new_connection(socket_desc);
    }
    pid_t pid = fork();
    if (pid == -1) {
        _exit(4);
    } else if (pid == 0) {
        printf("Hello from the child process (%d)!\n", getpid());
        fd_set local;

        time_t start_time = time(NULL);
        time_t last_message_time = time(NULL);
        bool not_closed = true;
        while(not_closed){
            char buffer[255];
            int counter;
            for(counter = 0; counter < 255; counter++)
                buffer[counter] = 0;
            int i = 0;
            bool reading = true;
            start_time = time(NULL);
            printf("%d: Waiting a message\n", getpid());
            select(1, (fd_set *) &ack, 0, 0, 0);
            do{
                char c;
                ssize_t x = recv(ack, &c, 1, 0);
                if (x > 0) {
                    buffer[i] = c;
                    if(buffer[i] == '.'){
                        i++;
                        buffer[i] = '\0';
                        reading = false;
                    }
                    i++;
                } else {
                    if(time(NULL) - last_message_time  > TIMEOUT){
                        reading = false;
                        printf("%d: Client has closed connection !\n", getpid());
                        not_closed = false;
                    }
                    usleep(500000);
                }
            }while(reading);
            if(i>0) {
                printf("%d: Recieve message : %s\n", getpid(), buffer);
                last_message_time = time(NULL);
            }
            usleep(500);
            if(not_closed&&i>0) {
                printf("%d: Respond on socket descriptor %d.\n", getpid(), ack);
                char *message = "Welcome to you, but I can't talk more.";
                write(ack, message, strlen(message));
            }
        }
        printf("End of world for %d\n", getpid());
        close(ack);
        _exit(EXIT_SUCCESS);
    } else {
        handle_new_connection(socket_desc);
    }
}