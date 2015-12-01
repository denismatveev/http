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
#define LREQUEST 1024
int main(int argc, char** argv)
{
  int sockfd,newsockfd;
  struct sockaddr_in serv_addr;
  serv_addr.sin_addr.in_addr=INADDR_ANY;
  serv_addr.sin_port=80;
  serv_addr.sin_family=AF_INET;
  char req_in[LREQUEST]; // for incoming requests
  char *request="GET / HTTP/1.1";

  char *response="HTTP/1.1 200 Ok"
                 "Server: My""
                 "Date: Tue, 01 Dec 2015 15:03:32 GMT"
                 "Content-Type: text/html; charset=utf-8
                 "Content-Length: 166
                 "Connection: close
                 "Content-Type: text/html; charset=utf-8\n"
                 "Content-Length: 166\n"
                 "Connection: close\n"
                 "                   \n"
                 "<html>\n"
                 "<head><title>Hi Men!</title></head>\n"
                 "<center><h1>Hi Men!</h1></center>\n"
                 "</body>\n"
                 "</html>\n";

  char *response_err="HTTP/1.1 400 Bad Request"
                     "Server: My"
                     "Date: Tue, 01 Dec 2015 15:04:56 GMT"
                     "Content-Type: text/html; charset=utf-8"
                     "Content-Length: 166"
                     "Connection: close"

                     "<html>"
                     "<head><title>400 Bad Request</title></head>"
                     "<body bgcolor=\"white\">"
                     "<center><h1>400 Bad Request</h1></center>"
                     "<hr><center>nginx</center>"
                     "</body>"
                     "</html>";


  if(!(sockfd = socket(AF_INET, SOCK_STREAM, 0)))
    {
      perror("Error opening socket\n");
      exit(1);
    }
  if(!(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))))
    {
      perror("Cannot bind()\n");
      exit(2);
    }


  if(!(listen(sockfd, 5)))
    {
      perror("Can't Listen()\n");
      exit(2);
    }

  if(!(accept(newsockfd, NULL, NULL)))
    {
      perror("Can'accept()\n");
      exit(2);
    }


   close(sockfd);

  return 0;
}

