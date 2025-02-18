#include "helpers.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>

int main(int argc, char *argv[]){

    if(argc != 3) {
        printf("usage: client <hostaddress> <your username>\n");
        exit(1);
    }
    if(strlen(argv[2]) > MAX_USERNAME_LEN){
        printf("username must be %d characters long at most\n", MAX_USERNAME_LEN);
        exit(1);
    }
    char client_name[MAX_USERNAME_LEN];
    strcpy(client_name, argv[2]);

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
    send(sockfd, client_name, sizeof(client_name) / sizeof(char), 0);

    if(fork() != 0) {
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
            printf("%s\n", outbuf);
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
}
