#include "helpers.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>

int main(int argc, char *argv[]){

    if(argc != 2) {
        printf("usage: client <hostaddress>\n");
        exit(1);
    }

    struct addrinfo hints, *res;

    bzero(&hints, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(argv[1], PORT, &hints, &res) == -1){
        perror("getaddrinfo error()");
        exit(-1);
    }

    int sockfd = getSock(res);
    if(connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("connect() error");
        exit(-1);
    }

    int parent_pid = getpid();
    fork();
    if(getpid() == parent_pid) {
        char outbuf[MAX_MSG_LEN];
        int recv_status;
        while(1){
            //receiving and outputting messages loop
            bzero(outbuf, MAX_MSG_LEN);
            recv_status = recv(sockfd, outbuf, MAX_MSG_LEN, 0);
            if(recv_status == -1) {
                perror("recv() error");
                exit(-1);
            }
            else if (recv_status == 0){
                printf("\nserver closed connection. Exiting.....\n");
                exit(0);
            }
            printf("\n%s\n", outbuf);
        }
    }
    else {
        char inbuf[MAX_MSG_LEN];
        while(1){
            //user input and sending loop
            bzero(inbuf, MAX_MSG_LEN);
            fgets(inbuf, MAX_MSG_LEN, stdin);
            if(send(sockfd, inbuf, MAX_MSG_LEN, 0) == -1) {
                perror("send() error");
                exit(-1);
            }
        }
    }

    freeaddrinfo(res);
}
