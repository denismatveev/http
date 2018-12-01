#include"listener.h"
#include "common.h"

int create_listener()
{
  int sockfd,r;
  char* inetaddr="0.0.0.0";// TODO should be taken from config
  int portno=80;
  struct sockaddr_in serv_addr;

  serv_addr.sin_port=htons(portno);
  serv_addr.sin_family=AF_INET;
  const int on = 1;

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
  if(!(r=inet_pton(AF_INET,inetaddr,&serv_addr.sin_addr.s_addr)))
    WriteLogPError("init_pton");
  else if(r < 0)
    {
      WriteLogPError("An error occured in address convertion from human readable to machine format");
      exit(EXIT_FAILURE);
    }
  else ;


  return sockfd;
}
