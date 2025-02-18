#pragma once
#include<netdb.h>
#include<pthread.h>

#define PORT "50500"
#define MAX_MSG_LEN 200
#define MAX_USERNAME_LEN 20

int getSock(struct addrinfo *res);

struct client{
    pthread_t thread;
    int sockfd;
    char name[MAX_USERNAME_LEN];
};

struct client_node {
    struct client data;
    struct client_node *prev;
    struct client_node *next;
};
