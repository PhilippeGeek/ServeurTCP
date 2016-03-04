//
// Created by tmoutier on 19/02/16.
//

#ifndef SERVEURTCP_SERVEUR_H
#define SERVEURTCP_SERVEUR_H

struct sockaddr_in init_server_descriptor(int port);
void enable_reuse_socket(int socket_desc);
void bind_server_socket(int socket_desc, int port);
void setup_server(int socket_desc, int port);

#endif //SERVEURTCP_SERVEUR_H
