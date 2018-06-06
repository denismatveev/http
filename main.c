//TODO web server logs to write
//TODO multithread or multiprocess
//TODO config
//TODO parse request and make response

#include"http.h"
#include "parse_config.h"
#define MAX_EVENTS 10
char *cfgFile = NULL;
char *cfgFilePath="/etc/lhttpd/lhttpd.conf";//default config file name

int main(int argc, char** argv)
{
  socklen_t clilen;
  int sockfd,r, conn_sock, nfds, epollfd, n;
  char* inetaddr="0.0.0.0";
  int portno=80;
  struct sockaddr_in serv_addr, cli_addr;
  serv_addr.sin_port=htons(portno);
  serv_addr.sin_family=AF_INET;
  struct epoll_event ev, events[MAX_EVENTS];
  const int on = 1;
  pid_t pid;
  char c;
  config_t cfg;




  while ((c = getopt (argc, argv, ":c:")) != -1)
  {
    switch (c)
    {
      case 'c':
        cfgFile = optarg;
        break;
      case '?':
        fprintf(stderr, "Unknown option -%c\n", optopt);
        exit(EXIT_FAILURE);
      case ':':
        fprintf(stderr, "Option -%c requires an argument\n", optopt);
        exit(EXIT_FAILURE);
      default:
        exit(EXIT_FAILURE);
    }
  }
  if(cfgFile == NULL)
    cfgFile=cfgFilePath;


  if((parse_cfg(&cfg,cfgFile) == -1))
  {
    WriteLog("Using default config\n");
    default_cfg(&cfg);
  }



  pid=fork();//child process

  if(pid == -1)
  {
    fprintf(stderr, "Error starting daemon %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
  if(pid)
  {
    fprintf(stderr,"[Daemon] Started OK, My PID = %i\n", pid);
    exit(EXIT_SUCCESS);
  }
  /* further code is executing in child process */
  if((setsid()) < 0)
  {
    fprintf(stderr,"[Daemon] An Error occured. Stop\n");
    exit(EXIT_FAILURE);
  }
  umask(0);

  if((chdir("/")) < 0)
  {
    fprintf(stderr,"[Daemon] Can't change directory\n");
    exit(EXIT_FAILURE);
  }

  fclose(stderr);
  fclose(stdin);
  fclose(stdout);
  if(!(r=inet_pton(AF_INET,inetaddr,&serv_addr.sin_addr.s_addr)))
    WriteLogPError("init_pton");
  else if(r < 0)
  {
    WriteLogPError("An error occured in address convertion from human readable to machine format");
    exit(EXIT_FAILURE);
  }
  else ;


  memset(&serv_addr.sin_zero,0,8);


  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
  {
    WriteLogPError("Error opening socket");
    exit(EXIT_FAILURE);
  }
  if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) // use it before bind(). It provides immediately server restart before TIME_WAIT is expired
    WriteLogPError("Error calling setsockopt");
  if((bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0))
  {
    WriteLogPError("Cannot bind socket");
    exit(EXIT_FAILURE);
  }

  // setnonblocking(sockfd);
  if((listen(sockfd, 255)) < 0)
  {
    WriteLogPError("Can't Listen socket");
    exit(EXIT_FAILURE);
  }
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
        //             setnonblocking(conn_sock);//if set non-blocking mode we need to use edge-triggered epoll mode and check EAGAIN in while loop to get all incoming data. it is asynchronous mode
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

        process_request(events[n].data.fd,&cli_addr);
        shutdown(events[n].data.fd, SHUT_RD); //shutdown is needed only if request for disconnect came
      }
    }
  }

  close(sockfd);


  return 0;
}

