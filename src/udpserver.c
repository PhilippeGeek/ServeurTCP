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

        byte port = (byte) rand();

        n = (int) sendto(sockfd, buf, 1, 0, (const struct sockaddr *) &clientaddr, (socklen_t) clientlen);

        if(n < 0)
            printf("Can not send port to client\n");

        int client = fork();

        if(client<0){
            error("Can not force the fork !\n");
        }
        if(client == 0){
            bool is_talking = true;
            int p = BASE_PORT+port;

            sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (sockfd < 0)
                error("ERROR opening socket");

            optval = 1;
            setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                       (const void *)&optval , sizeof(int));

            bzero((char *) &serveraddr, sizeof(serveraddr));
            serveraddr.sin_family = AF_INET;
            serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
            serveraddr.sin_port = htons((unsigned short) p);

            if (bind(sockfd, (struct sockaddr *) &serveraddr,
                     sizeof(serveraddr)) < 0)
                error("ERROR on binding");

            clientlen = sizeof(clientaddr);

            while(is_talking){
                n = (int) recvfrom(sockfd, buf, 1, 0,
                                   (struct sockaddr *) &clientaddr, (socklen_t *) &clientlen);
                if (n < 0)
                    error("ERROR in recvfrom");
                if(*buf != ACK)
                    is_talking = false;

                sprintf(buf,"Hello my name is Jarvis!\n");
                n = (int) sendto(sockfd, buf, strlen(buf), 0, (const struct sockaddr *) &clientaddr, (socklen_t) clientlen);
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