#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<errno.h>

#define PORT "3490"
#define BACKLOG 10
#define MAX_MSG_LEN 100

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

    while(1){
        new_fd = accept(sockfd, (struct sockaddr *) &new_addr, &sa_storage_len);
        send(new_fd, "vam conectat", MAX_MSG_LEN, 0);
        //TODO
    }


    freeaddrinfo(res);
}

int getSock(struct addrinfo *res){
    int sock;
    
    for(struct addrinfo *p = res; p != NULL; p = p->ai_next) {
        if((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            continue;
        }
        else {
            freeaddrinfo(res);
            res = p;
            return sock;
        }
    }
    fprintf(stderr, "socket() failed on all results from getaddrinfo()\n");
    exit(-1);
}
