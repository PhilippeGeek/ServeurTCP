
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>

int main(){
    int socket_desc = socket(PF_INET, SOCK_STREAM, 0);
    if(socket_desc == -1){
        fprintf(stderr, "Can not open an IPv4 socket ! Closing now.");
        exit(1);
    }
    printf("Yeah ! Socket is opened with descriptor %d", socket_desc);
    return 0;
}