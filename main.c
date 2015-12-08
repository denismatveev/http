#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//http://www.linuxhowtos.org/C_C++/socket.htm
//The steps involved in establishing a socket on the server side are as follows:

//Create a socket with the socket() system call
//Bind the socket to an address using the bind() system call. For a server socket on the Internet, an address consists of a port number on the host machine.
//Listen for connections with the listen() system call
//Accept a connection with the accept() system call. This call typically blocks until a client connects with the server.
//Send and receive data
/*
 *
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
int bind(int sockfd, struct sockaddr *addr, int addrlen);
int listen(int sockfd, int backlog);

int accept(int sockfd, void *addr, int *addrlen);
*/
#define LREQUEST 1024
int main(int argc, char** argv)
{
  int sockfd,newsockfd,n_bytes_read,portno=80;
  struct sockaddr_in serv_addr;
  socklen_t sizepeer=sizeof(serv_addr);
  serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  serv_addr.sin_port=htons(portno);
  serv_addr.sin_family=AF_INET;
  char req_in[LREQUEST]; // for incoming requests
  char *request="GET / HTTP/1.1";

  char *response="HTTP/1.1 200 Ok\n"
                 "Server: My\n"
                 "Date: Tue, 01 Dec 2015 15:03:32 GMT\n"
                 "Content-Type: text/html; charset=utf-8\n"
                 "Content-Length: 166\n"
                 "Connection: close\n"
                 "Content-Type: text/html; charset=utf-8\n"
                 "Content-Length: 166\n"
                 "Connection: close\n"
                 "                   \n"
                 "<html>\n"
                 "<head><title>Hi Men!</title></head>\n"
                 "<center><h1>Hi Men!</h1></center>\n"
                 "</body>\n"
                 "</html>\n";

  char *response_err="HTTP/1.1 400 Bad Request\n"
                     "Server: My\n"
                     "Date: Tue, 01 Dec 2015 15:04:56 GMT\n"
                     "Content-Type: text/html; charset=utf-8\n"
                     "Content-Length: 166\n"
                     "Connection: close\n"

                     "<html>\n"
                     "<head><title>400 Bad Request</title></head>\n"
                     "<body bgcolor=\"white\">\n"
                     "<center><h1>400 Bad Request</h1></center>\n"
                     "<hr><center>http server</center>\n"
                     "</body>\n"
                     "</html>\n";


  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
      perror("Error opening socket\n");
      exit(1);
    }
  if((bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0))
    {
      perror("Cannot bind socket\n");
      exit(2);
    }


  if((listen(sockfd, 5)) < 0)
    {
      perror("Can't Listen socket\n");
      exit(2);
    }

  if(((newsockfd = accept(sockfd, (struct sockaddr *) &serv_addr, &sizepeer) < 0)))
    {
      perror("Can't accept socket\n");
      exit(2);
    }


  if((n_bytes_read=read(newsockfd,req_in,LREQUEST)) < 0)
    {
      perror("Can't read from socket\n");

    }
  if((strcasecmp(req_in,request)))
      write(newsockfd,response,1024);
  else
    write(newsockfd, response_err, 1024);


   close(sockfd);
   close(newsockfd);

  return 0;
}

