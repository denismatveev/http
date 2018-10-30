#ifndef IOWORKER_H
#define IOWORKER_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdlib.h>
#define MAX_EVENTS 10

void create_ioworker(int sock);





#endif //IOWORKER_H
