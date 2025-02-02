#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<errno.h>

#define PORT "3490"

int getSock(struct addrinfo *node);

int main(){
    
    struct addrinfo hints, *res;
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(NULL, PORT, &hints, &res) == -1){
        fprintf(stderr, "getaddrinfo() error: %s\n", strerror(errno));
        exit(-1);
    }

    //struct addrinfo *emptyNode = malloc(sizeof(struct addrinfo));
    //emptyNode->ai_next = NULL;
    //struct addrinfo *emptyNode2 = malloc(sizeof(struct addrinfo));
    //emptyNode2->ai_next = emptyNode;


    sockfd = getSock(res);
    if(bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind() error");
        exit(1);
    }


    freeaddrinfo(res);
}

int getSock(struct addrinfo *res){
    int sock;
    
    for(struct addrinfo *p = res; p != NULL; p = p->ai_next) {
        if((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            printf("failed one result, moving forward\n");
            continue;
        }
        else {
            res = p;
            return sock;
        }
    }
    fprintf(stderr, "socket() failed on all results from getaddrinfo()\n");
    exit(-1);
}
