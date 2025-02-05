#include<sys/socket.h>
#include<netdb.h>
#include<stdlib.h>
#include<stdio.h>

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
