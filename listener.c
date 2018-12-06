#include"listener.h"
#include "common.h"
#include "parse_config.h"

int create_listener()
{
  config_t cfg;

  int sockfd;
  struct sockaddr_in serv_addr;

  serv_addr.sin_port=cfg.listen.sin_port;
  serv_addr.sin_family=cfg.listen.sin_family;
  serv_addr.sin_addr=cfg.listen.sin_addr;
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

  return sockfd;
}
