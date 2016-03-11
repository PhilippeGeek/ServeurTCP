//
// Created by Philippe VIENNE on 19/02/2016.
//

#ifndef SERVEURTCP_CONSTANTS_H
#define SERVEURTCP_CONSTANTS_H

typedef unsigned char byte;
#define REUSE_SOCKETS 1
#define SERVER_PORT 3456
#define TIMEOUT 15
#define CLIENT_PORT 1645
#define ACK 0x00000001
#define SYN 0x00000010
#define SYN_ACK ACK+SYN
#define BASE_PORT 10512

#endif //SERVEURTCP_CONSTANTS_H
