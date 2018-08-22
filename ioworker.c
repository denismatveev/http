#include"ioworker.h"
#include "common.h"
#include "http.h"
#include "pthread.h"
#include "stdlib.h"

static pthread_t **pthread_array;
static pthread_t *p;
static pthread_attr_t attr;


int create_ioworkers(int sock, int number_pthreads)
{

  int i;

  if((pthread_array=(pthread_t**)calloc(number_pthreads, sizeof(pthread_t*))) == NULL)
    {
      WriteLogPError("Allocating memory for threads");
      return(EXIT_FAILURE);
    }

  for(i=0; i < number_pthreads; i++)
    {
      if((p=(pthread_t*)malloc(sizeof(pthread_t))) == NULL)
        {
          WriteLogPError("malloc of pthread_t");
          return(EXIT_FAILURE);
        }

      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
      pthread_create(p, &attr, start_pthread_ioworker, &sock);
      pthread_array[i] = p;
    }
}

void destroy_ioworkers(int number_pthreads)
{
  int i;

  for(i=0; i < number_pthreads; i++)
    {
      free(pthread_array[i]);
    }
  pthread_attr_destroy(&attr);
  free(pthread_array);
}

void* start_pthread_ioworker(void* sock)
{
  int sockfd = *(int*)sock;
  int conn_sock, nfds, epollfd, n;
  struct epoll_event ev, events[MAX_EVENTS];
  epollfd = epoll_create1(0);
  ev.events = EPOLLIN;
  ev.data.fd = sockfd;

  struct sockaddr_in cli_addr;
  socklen_t clilen;

  epollfd = epoll_create1(0);
  if (epollfd == -1)
    {
      WriteLogPError("epoll_create1");
      exit(EXIT_FAILURE);
    }
  ev.events = EPOLLIN;
  ev.data.fd = sockfd;


  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
    {
      WriteLogPError("epoll_ctl");
      exit(EXIT_FAILURE);
    }

  for (;;)
    {
      nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
      if (nfds == -1)
        {
          WriteLogPError("epoll_wait");
          exit(EXIT_FAILURE);
        }

      for (n = 0; n < nfds; ++n)
        {
          if (events[n].data.fd == sockfd)
            {
              conn_sock = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
              if (conn_sock == -1)
                {
                  WriteLogPError("accept");
                  exit(EXIT_FAILURE);
                }
              //setnonblocking(conn_sock);//if set non-blocking mode we need to use edge-triggered epoll mode and check EAGAIN in while loop to get all incoming data. it is asynchronous mode
              ev.events = EPOLLIN | EPOLLPRI;
              ev.data.fd = conn_sock;
              if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,&ev) == -1)
                {
                  WriteLogPError("epoll_ctl: conn_sock");
                  exit(EXIT_FAILURE);
                }
            }
          else
            {
              //TODO

              // request handler
              // we have file descriptor
              // ioworker have to read data from fd, create job
              // worker have to get job, find file(e.g. index.html) and reply

              shutdown(events[n].data.fd, SHUT_RD); //shutdown is needed only if got a request for disconnecting
            }
        }
    }
}
