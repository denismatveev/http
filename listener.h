#ifndef LISTENER_H
#define LISTENER_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdlib.h>



int create_listener(void);



#endif //LISTENER_H
