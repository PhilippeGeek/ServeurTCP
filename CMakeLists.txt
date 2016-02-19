cmake_minimum_required(VERSION 3.3)
project(ServeurTCP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SERVEUR_SOURCE_FILES src/serveur.c src/serveur.h)
set(CLIENT_SOURCE_FILES src/client.c src/client.h)
add_executable(serveur ${SERVEUR_SOURCE_FILES})
add_executable(client ${CLIENT_SOURCE_FILES})