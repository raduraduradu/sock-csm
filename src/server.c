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
#include<pthread.h>

#define BACKLOG 10
#define SEM_SENDER_FN "/sendersem"

//#define INIT_CLIENT {->prev = NULL, ->next = NULL}
//#define MAX_CLIENTS 10

struct client_node* init_client();
void removeNode(struct client_node* node);

//int client_sockets[MAX_CLIENTS];
//pthread_t client_threads[MAX_CLIENTS];

struct client_node *clients_dll; //doubly linked list node for client struct
struct client_node *current_client;

char last_msg[MAX_MSG_LEN];

void * handle_client(void *arg) {
    struct client_node *thisNode = (struct client_node*) arg;
    while(1){
        //lock
        //recv into last_msg
        int recvstatus = recv(thisNode->data.sockfd, last_msg, MAX_MSG_LEN, 0);
        if (recvstatus == -1) {
            printf("cam da\n");
            perror("recv() error");
            exit(-1);
        }
        else if (recvstatus == 0){
            pthread_t thisThread = thisNode->data.thread;
            removeNode(thisNode);
            printf("a client has disconnected\n");
            pthread_exit(&thisThread);
        }
        printf("%s\n", last_msg);
        //send last_msg to all clients
        for(struct client_node *p = clients_dll; p != NULL; p = p->next){
            send(p->data.sockfd, last_msg, MAX_MSG_LEN, 0);
        }
        //unlock
    }
}


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
    
    struct sockaddr_storage addr_storage;
    socklen_t sa_storage_len = sizeof(struct sockaddr_storage);

    clients_dll = init_client();
    current_client = clients_dll;

    current_client->data.sockfd = accept(listener_sfd, (struct sockaddr*) &addr_storage, &sa_storage_len);
    pthread_create(&(current_client->data.thread), NULL, handle_client, current_client);

    int new_sfd;
    while(1) {
        new_sfd = accept(listener_sfd, (struct sockaddr*) &addr_storage, &sa_storage_len);
        current_client->next = init_client();
        current_client->next->prev = current_client;
        current_client = current_client->next;

        current_client->data.sockfd = new_sfd;
        pthread_create(&(current_client->data.thread), NULL, handle_client, current_client);
    }
}

struct client_node* init_client(){
    struct client_node *node = malloc(sizeof(struct client_node));
    node->prev = NULL;
    node->next = NULL;
}

void removeNode(struct client_node* node){
    if(node->prev == NULL){
        clients_dll = node->next;
        clients_dll->prev = NULL;
    }
    else if (node->next == NULL){
        current_client = node->prev;
        current_client->next = NULL;
    }
    else{
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    free(node);
    node = NULL;
}
