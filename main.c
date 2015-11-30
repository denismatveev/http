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
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  portno=80;
  char req_in[LREQUEST]; // for incoming requests
  char *request="GET / HTTP/1.1";
  char *response="Content-Type: text/html; charset=utf-8\n"
                 "Content-Length: 166\n"
                 "Connection: close\n"
                 "                   \n"
                 "<html>\n"
                 "<head><title>Hi Men!</title></head>\n"
                 "<center><h1>Hi Men!</h1></center>\n"
                 "</body>\n"
                 "</html>\n";



  if(!(sockfd = socket(AF_INET, SOCK_STREAM, 0)))
  {
     perror("Error opening socket\n");
     exit(1);
}



     return 0;
}

