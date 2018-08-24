#ifndef _LISTENER_H
#define _LISTENER_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdlib.h>
#include "jobs_queue.h"
#define MAX_EVENTS 10

int create_listener();
int start_ioworker(int sock);
int data_to_jobs_queue(int, jobs_queue_t*);
#endif //_LISTENER_H
