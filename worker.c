#include"worker.h"
#include "common.h"
#include "http.h"
#include "pthread.h"
#include "stdlib.h"
#include "jobs_queue.h"


void create_ioworker(int sockfd)
{
  int conn_sock, nfds, epollfd, n;
  struct epoll_event ev, events[MAX_EVENTS];
  epollfd = epoll_create1(0);
  ev.events = EPOLLIN;
  ev.data.fd = sockfd;

  struct sockaddr_in cli_addr;
  socklen_t clilen;

  raw_client_data_t *rd;
  size_t bytes_read;

  epollfd = epoll_create1(0);
  if (epollfd == -1)
    {
      WriteLogPError("epoll_create1");
      exit(EXIT_FAILURE);
    }


  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
    {
      WriteLogPError("epoll_ctl");
      exit(EXIT_FAILURE);
    }
  //TODO the cycle below place into dedicated thread
  for (;;)
    {
      nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
      if (nfds == -1)
        {
          WriteLogPError("epoll_wait");
          exit(EXIT_FAILURE);
        }

      for (n = 0; n < nfds; ++n)
        { //event on listening socket
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
              //event on accepted socket


              rd = create_raw_data();
              rd->client_socket=events[n].data.fd;
              bytes_read=read(rd->client_socket, rd->initial_data,INITIAL_DATA_SIZE);
              rd->initial_data[bytes_read+1] = 0;//adds 0 to the end of data

              //run workers in dedicated threads



              delete_raw_data(rd);

              shutdown(events[n].data.fd, SHUT_RD); //shutdown is needed only if got a request for disconnecting
            }
        }
    }
}


