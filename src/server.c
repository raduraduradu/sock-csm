#include "helpers.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<pthread.h>
#include<stdbool.h>

#define BACKLOG 10

struct client_node* init_clientnode();
void removeNode(struct client_node* node);

struct client_node *clients_dll; //doubly linked list node for client struct
struct client_node *current_client;

char last_msg[MAX_MSG_LEN];

pthread_mutex_t sender_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t removeNode_lock = PTHREAD_MUTEX_INITIALIZER;


void * handle_client(void *arg) {
    struct client_node *thisNode = (struct client_node*) arg;
    char inbuf[MAX_MSG_LEN];

    strcpy(inbuf, "succesfully connected to server\n");
    send(thisNode->data.sockfd, inbuf, MAX_MSG_LEN, 0);

    printf("%s has connected\n\n", thisNode->data.name);

    while(1){
        int recvstatus = recv(thisNode->data.sockfd, inbuf, MAX_MSG_LEN, 0);
        if (recvstatus == -1) {
            perror("recv() error");
            exit(-1);
        }
        else if (recvstatus == 0){
            struct client thisClient = thisNode->data;

            pthread_mutex_lock(&removeNode_lock);
            removeNode(thisNode);
            pthread_mutex_unlock(&removeNode_lock);

            printf("%s has disconnected\n\n", thisClient.name);
            pthread_exit(&(thisClient.thread));
        }
        printf("[%s] %s\n", thisNode->data.name, inbuf);

        pthread_mutex_lock(&sender_lock);
        sprintf(last_msg, "[%s]: %s", thisNode->data.name, inbuf);
        //strcpy(last_msg, inbuf);
        //send last_msg to all clients
        for(struct client_node *p = clients_dll; p != NULL; p = p->next){
            send(p->data.sockfd, last_msg, MAX_MSG_LEN, 0);
        }
        pthread_mutex_unlock(&sender_lock);
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

    clients_dll = NULL;
    
    int new_sfd;
    while(1) {
        new_sfd = accept(listener_sfd, (struct sockaddr*) &addr_storage, &sa_storage_len);

        if (clients_dll == NULL) {
            clients_dll = init_clientnode();
            current_client = clients_dll;
        }
        else {
            current_client->next = init_clientnode();
            current_client->next->prev = current_client;
            current_client = current_client->next;
        }
        
        recv(new_sfd, current_client->data.name, MAX_USERNAME_LEN, 0);
        current_client->data.sockfd = new_sfd;
        pthread_create(&(current_client->data.thread), NULL, handle_client, current_client);
    }
}

struct client_node* init_clientnode(){
    struct client_node *node = malloc(sizeof(struct client_node));
    node->prev = NULL;
    node->next = NULL;

    return(node);
}

void removeNode(struct client_node* node){
    bool noPrev = node->prev == NULL;
    bool noNext = node->next == NULL;
    
    if(noPrev && noNext){
        clients_dll = NULL;
    }
    else if(noPrev){
        clients_dll = node->next;
        clients_dll->prev = NULL;
    }
    else if(noNext){
        current_client = node->prev;
        current_client->next = NULL;
    }
    else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    free(node);
    node = NULL;
}
