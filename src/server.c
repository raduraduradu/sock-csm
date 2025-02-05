#include "helpers.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<errno.h>

#define BACKLOG 10

int getSock(struct addrinfo *node);

int main(){
    
    struct addrinfo hints, *res;
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(NULL, PORT, &hints, &res) == -1){
        perror("getaddrinfo() error");
        exit(-1);
    }

    sockfd = getSock(res);
    if(bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind() error");
        exit(-1);
    }
    if(listen(sockfd, BACKLOG) == -1) {
        perror("listen() error");
        exit(-1);
    }
    
    struct sockaddr_storage new_addr;
    socklen_t sa_storage_len = sizeof(struct sockaddr_storage);

    int new_fd;

    char message[] = "vam conectat";
    while(1){
        new_fd = accept(sockfd, (struct sockaddr *) &new_addr, &sa_storage_len);
        send(new_fd, message, sizeof message, 0);
        //TODO
    }


    freeaddrinfo(res);
}
