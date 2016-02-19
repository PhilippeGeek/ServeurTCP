//
// Created by tmoutier on 19/02/16.
//

#ifndef SERVEURTCP_SERVEUR_H
#define SERVEURTCP_SERVEUR_H

struct sockaddr_in init_server_descriptor();
void enable_reuse_socket(int socket_desc);
void bind_server_socket(int socket_desc);
void setup_server(int socket_desc);

#endif //SERVEURTCP_SERVEUR_H
