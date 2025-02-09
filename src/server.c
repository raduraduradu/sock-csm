#include "helpers.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<errno.h>
#include<signal.h>
#include<sys/mman.h>
#include<semaphore.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>

#define BACKLOG 10
#define SEM_SENDER_FN "/sendersem"

int main(){
    
    struct addrinfo hints, *res;
    int listener_sfd;

    bzero(&hints, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(NULL, PORT, &hints, &res) == -1){
        perror("getaddrinfo() error");
        exit(-1);
    }

    listener_sfd = getSock(res);
    if(bind(listener_sfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind() error");
        exit(-1);
    }
    if(listen(listener_sfd, BACKLOG) == -1) {
        perror("listen() error");
        exit(-1);
    }
    
    struct sockaddr_storage new_addr;
    socklen_t sa_storage_len = sizeof(struct sockaddr_storage);

    int ch_sfd; //client handler socket file descriptor

    if (char (*last_msg)[MAX_MSG_LEN] = mmap(NULL, MAX_MSG_LEN,
                             PROT_READ | PROT_WRITE,
                             MAP_SHARED | MAP_ANON, -1, 0) == MAP_FAILED){
        perror("mmap() error");
        exit(2)
    }

    strcpy(*last_msg, "Succesfully connected to server");

    sem_unlink(SEM_SENDER_FN);
    if(sem_t *sender_sem = sem_open(SEM_SENDER_FN, O_CREAT, S_IRWXU, 1) == SEM_FAILED) {
        perror("sem_open() error");
        exit(3);
    }

    while(1){
        ch_sfd = accept(listener_sfd, (struct sockaddr *) &new_addr, &sa_storage_len);
        send(ch_sfd, *last_msg, strlen(*last_msg), 0);
        //fork a client handler
        fork();
        close(listener_sfd);
        int id = fork(); //fork into a receiving process and a sending process
        if (id != 0){
            char inbuf[MAX_MSG_LEN];
            while(1){
                int recvStatus = recv(ch_sfd, inbuf, MAX_MSG_LEN, 0);
                if(recvStatus == -1){
                    perror("recv error in client handler");
                    kill(id, SIGINT);
                    exit(-1);
                }else if (recvStatus == 0) {
                    kill(id, SIGINT);
                    exit(0);
                }
                strcpy(*last_msg, inbuf);

                printf("[%s]", *last_msg);
                //send signal that child process waits for
                sem_post(sender_sem);
            }
        }
        else {
            while(1){
                //wait for signal that *last_msg has changed
                sem_wait(sender_sem);
                send(ch_sfd, *last_msg, MAX_MSG_LEN, 0);
            }
        }
    }
    //TODO: handle SIGINT and errors, freeaddrinfo and close file descriptors
    //also add more error checking if needed
}
