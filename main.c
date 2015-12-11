#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>
//TODO web server logs to write
//TODO run as daemon
//TODO multithread or multiprocess
//TODO config
//TODO parse request and make response
#define LREQUEST 1024
int main(int argc, char** argv)
{
  socklen_t clilen;
  int sockfd,newsockfd,n_bytes_read,r;
  char* inetaddr="0.0.0.0";
  int portno=80;
  struct sockaddr_in serv_addr, cli_addr;
  size_t len_err_response;
  clilen=sizeof(cli_addr);
  char haddr[LREQUEST];
  serv_addr.sin_port=htons(portno);
  serv_addr.sin_family=AF_INET;
  char req_in[LREQUEST]; // for incoming requests
  char *message="Your IP address is: ";
  char response[LREQUEST];
  char *request="GET / HTTP/1.1";

  char *response_err="<html><body><center><h1>400 Bad Request</h1></center></body></html>\n";
  char *opentags="<html><body><center><h1>";
  char *closetags="</center></body></html>";

  r=inet_pton(AF_INET,inetaddr,&serv_addr.sin_addr.s_addr);
  if(!r)
    {
      fprintf(stderr,"Internet address is not valid\n");
      fprintf(stderr,"Using 0.0.0.0\n");
      inetaddr="0.0.0.0";
      if((r=inet_pton(AF_INET,inetaddr,&serv_addr.sin_addr.s_addr) < 0))
        {

          perror("An error occured in address covert from human readable to machine format");
          exit(2);

        }
    }
  else if(r < 0)
    {
      perror("An error occured in address covert from human readable to machine format");
      exit(2);
    }
  else ;


  memset(&serv_addr.sin_zero,0,8);// the same as bzero(). Stevens recommends
  memset(req_in,0,LREQUEST);// filling by zeroes

  len_err_response=strlen(response_err);

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
      perror("Error opening socket\n");
      exit(1);
    }
  if((bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0))
    {
      perror("Cannot bind socket");
      exit(2);
    }


  if((listen(sockfd, 255)) < 0)
    {
      perror("Can't Listen socket");
      exit(2);
    }

  while(1)
    {
      if(((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0))
        {
          perror("Can't accept socket");
          exit(2);
        }


      n_bytes_read=read(newsockfd,req_in,LREQUEST-1);

      if(n_bytes_read < 0)
        perror("Can't read from socket");

      if(!(strncasecmp(req_in,request,strlen(request))))
        {

          inet_ntop(AF_INET,&cli_addr.sin_addr,haddr,LREQUEST);
          snprintf(response,LREQUEST,"%s%s%s%s\n",opentags,message,haddr,closetags);
          write(newsockfd, response,strlen(response));
        }
      else
        write(newsockfd, response_err, len_err_response);

      close(newsockfd);
    }
  close(sockfd);


  return 0;
}

