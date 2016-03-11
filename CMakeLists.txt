cmake_minimum_required(VERSION 3.3)
project(ServeurTCP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SERVEUR_SOURCE_FILES src/serveur.c src/serveur.h src/constants.h)
set(SERVEUR_UDP_SOURCE_FILES src/udpserveur.c src/udpserveur.h src/constants.h)
set(CLIENT_SOURCE_FILES src/client.c src/client.h src/constants.h)
set(CLIENT_UDP_SOURCE_FILES src/udpclient.c src/udpclient.h src/constants.h)
add_executable(serveur ${SERVEUR_SOURCE_FILES})
add_executable(client ${CLIENT_SOURCE_FILES})
add_executable(udpserveur ${SERVEUR_SOURCE_FILES})
add_executable(udpclient ${CLIENT_SOURCE_FILES})