//
// Created by Philippe VIENNE on 11/03/2016.
//

#include "udpserver.h"
#include "constants.h"

/*
 * udpserver.c - A simple UDP echo server
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define BUFSIZE 1024

bool run = true;

/*
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char **argv) {
    int sockfd; /* socket */
    int portno; /* port to listen on */
    int clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    char buf[BUFSIZE]; /* message buf */
    char *hostaddrp; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
    int n; /* message byte size */

    /*
     * check command line arguments
     */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[1]);

    /*
     * socket: create the parent socket
     */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* setsockopt: Handy debugging trick that lets
     * us rerun the server immediately after we kill it;
     * otherwise we have to wait about 20 secs.
     * Eliminates "ERROR on binding: Address already in use" error.
     */
    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
               (const void *)&optval , sizeof(int));

    /*
     * build the server's Internet address
     */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);

    /*
     * bind: associate the parent socket with a port
     */
    if (bind(sockfd, (struct sockaddr *) &serveraddr,
             sizeof(serveraddr)) < 0)
        error("ERROR on binding");

    /*
     * main loop: wait for a datagram, then echo it
     */
    clientlen = sizeof(clientaddr);
    while (run) {

        /*
         * recvfrom: receive a UDP datagram from a client
         */
        bzero(buf, BUFSIZE);
        n = (int) recvfrom(sockfd, buf, 1, 0,
                           (struct sockaddr *) &clientaddr, (socklen_t *) &clientlen);
        if (n < 0)
            error("ERROR in recvfrom");
        if(*buf != SYN)
            continue;

        hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
                              sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        if (hostp == NULL)
            error("ERROR on gethostbyaddr");
        hostaddrp = inet_ntoa(clientaddr.sin_addr);
        if (hostaddrp == NULL)
            error("ERROR on inet_ntoa\n");
        printf("Connection started with %s (%s)\n",
               hostp->h_name, hostaddrp);

        *buf = SYN_ACK;

        n = (int) sendto(sockfd, buf, 1, 0,
                         (struct sockaddr *) &clientaddr, (socklen_t) clientlen);

        n = (int) recvfrom(sockfd, buf, 1, 0,
                           (struct sockaddr *) &clientaddr, (socklen_t *) &clientlen);
        if (n < 0)
            error("ERROR in recvfrom");
        if(*buf != ACK)
            continue;

        printf("3-way handshake ended with success !\n");

        byte port = (byte) (rand()%256);

        if(n < 0)
            printf("Can not send port to client\n");

        int client = fork();

        if(client<0){
            error("Can not force the fork !\n");
        }
        if(client == 0){
            bool is_talking = true;
            int p = BASE_PORT+port;

            printf("In child %d\n", getpid());

            int dedicated_sockfd;
            int dedicated_portno = BASE_PORT+port;
            int dedicated_clientlen;
            struct sockaddr_in dedicated_serveraddr;
            struct sockaddr_in dedicated_clientaddr;

            dedicated_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (dedicated_sockfd < 0)
                error("ERROR opening socket");

            optval = 1;
            setsockopt(dedicated_sockfd, SOL_SOCKET, SO_REUSEADDR,
                       (const void *)&optval , sizeof(int));

            bzero((char *) &dedicated_serveraddr, sizeof(dedicated_serveraddr));
            dedicated_serveraddr.sin_family = AF_INET;
            dedicated_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
            dedicated_serveraddr.sin_port = htons((unsigned short) dedicated_portno);

            if (bind(dedicated_sockfd, (struct sockaddr *) &dedicated_serveraddr,
                     sizeof(dedicated_serveraddr)) < 0)
                error("ERROR on binding");

            dedicated_clientlen = sizeof(dedicated_clientlen);

            // Send the port on the old socket
            n = (int) sendto(sockfd, &port, 1, 0, (const struct sockaddr *) &clientaddr, (socklen_t) clientlen);

            // Wait ACK on the new port
            n = (int) recvfrom(dedicated_sockfd, buf, 1, 0,
                               (struct sockaddr *) &dedicated_clientaddr, (socklen_t *) &dedicated_clientlen);
            if (n < 0)
                error("ERROR in recvfrom");
            if(*buf != ACK)
                is_talking = false;

            while(is_talking){

                // Wait for request

                size_t size = sizeof(size_t);
                recvfrom(dedicated_sockfd, &size, sizeof(size_t), 0, (struct sockaddr *) &dedicated_clientaddr,
                         (socklen_t *) &dedicated_clientlen);
                printf("Will read a request %d\n", (int) size);

                bzero(buf, BUFSIZE);
                recvfrom(dedicated_sockfd, &buf, size, 0, (struct sockaddr *) &dedicated_clientaddr,
                         (socklen_t *) &dedicated_clientlen);

                printf("Asked %s\n", buf);

                // Open the file

                FILE* fp = fopen(buf,"r"); // read mode

                if( fp == NULL )
                {
                    printf("Can not open %s\n", buf);
                    size = 0;
                    n = (int) sendto(dedicated_sockfd, &size, sizeof(size), 0, (const struct sockaddr *) &dedicated_clientaddr, (socklen_t) dedicated_clientlen);
                    continue;
                }

                int buffer = 1024;
                byte data[1024];
                int b;

                do{
                    buffer = 1024;
                    char byte;
                    bzero(data, 1024);
                    while(buffer != 0 && (b = fgetc(fp)) != EOF){
                        data[1024-buffer] = (unsigned char)b;
                        buffer--;
                    }
                    int buffer_size = 1024 - buffer;
                    (int) sendto(dedicated_sockfd, &buffer_size, sizeof(size), 0,
                                 (const struct sockaddr *) &dedicated_clientaddr, (socklen_t) dedicated_clientlen);
                    (int) sendto(dedicated_sockfd, data, (size_t) buffer_size, 0,
                                 (const struct sockaddr *) &dedicated_clientaddr, (socklen_t) dedicated_clientlen);
                    n = (int) recvfrom(dedicated_sockfd, buf, 1, 0,
                                       (struct sockaddr *) &dedicated_clientaddr, (socklen_t *) &dedicated_clientlen);
                    if (n < 0)
                        error("ERROR in recvfrom");
                }while(buffer == 0);

                fclose(fp);

                // Serve the file


                size = strlen(buf);
                n = (int) sendto(dedicated_sockfd, &size, sizeof(size), 0, (const struct sockaddr *) &dedicated_clientaddr, (socklen_t) dedicated_clientlen);
                n = (int) sendto(dedicated_sockfd, buf, strlen(buf), 0, (const struct sockaddr *) &dedicated_clientaddr, (socklen_t) dedicated_clientlen);
                if(n<0)
                    is_talking = false;
                sleep(1);
            }
            close(sockfd);
            exit(0);
        } else {
            printf("Client has been redirected to port %d\n", BASE_PORT+port);
        }

    }
}