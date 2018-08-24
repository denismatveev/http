#include"listener.h"
#include "common.h"
#include "jobs_queue.h"
#include "http.h"
extern jobs_queue_t* jobs;
int create_listener()
{
  int sockfd,r;
  char* inetaddr="0.0.0.0";
  int portno=80;
  struct sockaddr_in serv_addr;

  serv_addr.sin_port=htons(portno);
  serv_addr.sin_family=AF_INET;
  const int on = 1;

  memset(&serv_addr.sin_zero,0,8);


  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
  {
    WriteLogPError("Error opening socket");
    return sockfd;
  }
  if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) // use it before bind(). It provides immediately server restart before TIME_WAIT is expired
    WriteLogPError("Error calling setsockopt");
  if((bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0))
  {
    WriteLogPError("Cannot bind socket");
    return -1;
  }

  // setnonblocking(sockfd);
  if((listen(sockfd, 255)) < 0)
  {
    WriteLogPError("Can't Listen socket");
    return -1;
  }
  if(!(r=inet_pton(AF_INET,inetaddr,&serv_addr.sin_addr.s_addr)))
    WriteLogPError("init_pton");
  else if(r < 0)
  {
    WriteLogPError("An error occured in address convertion from human readable to machine format");
    return -1;
  }
  else ;

  start_ioworker(sockfd);

}

int start_ioworker(int sockfd)
{
  int conn_sock, nfds, epollfd, n;
  struct epoll_event ev, events[MAX_EVENTS];
  epollfd = epoll_create1(0);
  ev.events = EPOLLIN;
  ev.data.fd = sockfd;
  struct sockaddr_in cli_addr;
  socklen_t clilen;

  epollfd = epoll_create1(0);// 0 is a flag
  if (epollfd == -1)
  {
    WriteLogPError("epoll_create1");
    goto clean;

  }


  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
  {
    WriteLogPError("epoll_ctl");
    goto clean;
  }

  for (;;)
  {
    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1)
    {
      WriteLogPError("epoll_wait");
      goto clean;
    }

    for (n = 0; n < nfds; ++n)
    {
      if (events[n].data.fd == sockfd)
      {
        conn_sock = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (conn_sock == -1)
        {
          WriteLogPError("accept");
          goto clean;

        }
        //setnonblocking(conn_sock);//if set non-blocking mode we need to use edge-triggered epoll mode and check EAGAIN in while loop to get all incoming data. it is asynchronous mode
        ev.events = EPOLLIN | EPOLLPRI;
        ev.data.fd = conn_sock;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,&ev) == -1)
        {
          WriteLogPError("epoll_ctl: conn_sock");
          goto clean;
        }
      }
      else
      {
        data_to_jobs_queue(conn_sock, jobs);
        shutdown(events[n].data.fd, SHUT_RD); //shutdown is needed only if got a request for disconnecting
      }
    }
  }

clean:
  close_jobs_queue(jobs);
  return -1;
}

int data_to_jobs_queue(int conn_sock, jobs_queue_t* jobs)
{
  job_t* job;
  char buff[LREQUEST];
  //TODO
  ssize_t ssize;
  ssize = read(conn_sock, buff, LREQUEST);
  job = create_job(buff);
  push_job(jobs, job);
  //ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);

  return ssize;

}
