#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <sys/wait.h>
#include "constants.h"

int running = 1;

void enable_reuse_socket(int socket_desc);

bool parseIP(char *ip_addr, unsigned int *ip);

bool parsePort(char* port_given, int *port);

int main(int argc, char* argv[]){
    unsigned int ip = INADDR_LOOPBACK;
    int port = SERVER_PORT;
    bool exit_due_to_arg_error;
    switch(argc){
        case 1:
            fprintf(stderr, "You should, at least, give an IP for the server.");
            exit_due_to_arg_error = true;
            break;
        case 2:
            fprintf(stdout, "The argument %s will be interpreted as an IP. And port is supposed to be %d.", argv[1], SERVER_PORT);
            exit_due_to_arg_error = parseIP(argv[1], &ip);
            break;
        case 3:
            exit_due_to_arg_error = parsePort(argv[2], &port) && parseIP(argv[1], &ip);
            break;
        default:
            fprintf(stderr, "Too many arguments we can not handle that!");
            exit_due_to_arg_error = true;
            break;
    }
    if(exit_due_to_arg_error){
        exit(2);
    }
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
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr = htonl(ip); /* set destination IP number - localhost, 127.0.0.1*/

    printf("Socket is opened with description: %d\n", socket_desc);

    int i = connect(socket_desc, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    if(i < 0)
        printf("Failed to connect to server");

    char *string = "Hello.";
    if(write(socket_desc, string, 6) != 6){
        printf("Failed to connect to server\n");
        exit(3);
    }
    bool reading = true;
    bool not_closed = true;
    char buffer[255];
    int j = 0;
    do{
        char c;
        ssize_t x = read(socket_desc, &c, 1);
        if (x > 0) {
            buffer[j] = c;
            if(buffer[j] == '.'){
                j++;
                buffer[j] = '\0';
                reading = false;
            }
            j++;
        } else {
            not_closed = false;
        }
    }while(reading);
    printf("%d: Recieve message : %s\n", getpid(), buffer);
    sleep(10);
    close(socket_desc);

    return 0;
}

bool parsePort(char* port_given, int *port) {
    bool exit_due_to_arg_error = false;
    (*port) = atoi(port_given);
    if(*port <= 0 || *port > 65535){
        fprintf(stderr, "Port (%s) is incorrect and we could not continue !", port_given);
        exit_due_to_arg_error = true;
    }
    return exit_due_to_arg_error;
}

bool parseIP(char *ip_addr, unsigned int *ip) {
    bool exit_due_to_arg_error=false;
    if(!inet_pton(AF_INET, ip_addr, ip)){
                fprintf(stderr, "IP (%s) is incorrect and we could not continue !", ip_addr);
                exit_due_to_arg_error = true;
            }
    return exit_due_to_arg_error;
}

void enable_reuse_socket(int socket_desc) {
        int reuse= REUSE_SOCKETS;
        setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    }