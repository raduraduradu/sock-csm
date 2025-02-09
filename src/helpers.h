#pragma once
#include<netdb.h>

#define PORT "50500"
#define MAX_MSG_LEN 100

int getSock(struct addrinfo *res);
