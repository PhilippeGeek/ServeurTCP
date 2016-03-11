/*
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include "constants.h"

#define BUFSIZE 1024

bool connect_to(int *sockfd, int portno, struct sockaddr_in *serveraddr, struct hostent *server, const char *hostname);

/*
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char **argv) {
    int sockfd, portno, n;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];

    /* check command line arguments */
    if (argc != 3) {
        fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
        exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serveraddr.sin_addr.s_addr, (size_t) server->h_length);
    serveraddr.sin_port = htons(portno);

    /* get a message from the user */
    bzero(buf, BUFSIZE);

    /* send the message to the server */
    serverlen = sizeof(serveraddr);
    (*buf) = SYN;
    n = (int) sendto(sockfd, buf, 1, 0, (const struct sockaddr *) &serveraddr, (socklen_t) serverlen);
    if (n < 0)
        error("ERROR in sendto");

    /* print the server's reply */
    n = (int) recvfrom(sockfd, buf, 1, 0, (struct sockaddr *) &serveraddr, (socklen_t *) &serverlen);
    if (n < 0)
        error("ERROR in SYN-ACK receive");

    if((*buf) != SYN_ACK)
        error("Cannot continue, the server refuse the ACK.");

    (*buf) = ACK;
    n = (int) sendto(sockfd, buf, 1, 0, (const struct sockaddr *) &serveraddr, (socklen_t) serverlen);

    if (n < 0)
        error("ERROR in ACK sending");

    printf("3-way handshake ended - Waiting Port\n");
    n = (int) recvfrom(sockfd, buf, 1, 0, (struct sockaddr *) &serveraddr, (socklen_t *) &serverlen);
    if (n < 0)
        error("ERROR in PORT receive");

    shutdown(sockfd, 2);

    connect_to(&sockfd, BASE_PORT+((byte)*buf), &serveraddr, server, hostname);

    (*buf) = ACK;
    n = (int) sendto(sockfd, buf, 1, 0, (const struct sockaddr *) &serveraddr, (socklen_t) serverlen);
    if (n < 0)
        error("ERROR in ACK sending for dedicated connection");

    printf("Connected to port %d\n", BASE_PORT+(*buf));

    bool is_talking = true;
    while(is_talking){
        n = (int) recvfrom(sockfd, buf, 1, 0, (struct sockaddr *) &serveraddr, (socklen_t *) &serverlen);
        is_talking = n>=0;
        printf("%s", buf);
    }

    return 0;
}

bool connect_to(int *sockfd, int portno, struct sockaddr_in *serveraddr, struct hostent *server, const char *hostname) {/* socket: create the socket */
    *sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
    }

    /* build the server's Internet address */
    bzero((char *) serveraddr, sizeof((*serveraddr)));
    (*serveraddr).sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&(*serveraddr).sin_addr.s_addr, (size_t) server->h_length);
    (*serveraddr).sin_port = htons(portno);
}