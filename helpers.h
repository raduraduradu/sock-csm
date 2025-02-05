#pragma once
#include<netdb.h>

#define PORT "3490"
#define MAX_MSG_LEN 100

int getSock(struct addrinfo *res);
