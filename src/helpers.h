#pragma once
#include<netdb.h>
#include<pthread.h>

#define PORT "50500"
#define MAX_MSG_LEN 100

int getSock(struct addrinfo *res);

struct client{
    pthread_t thread;
    int sockfd;
};

struct client_node {
    struct client data;
    struct client_node *prev;
    struct client_node *next;
};
