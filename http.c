#include"http.h"
#define LREQUEST 1024
#define MAX_EVENTS 10
void WriteLog(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsyslog(LOG_LOCAL0, format, args);
    va_end(args);

    return;
}
void WriteLogPError(const char * strerr)
{
    syslog(LOG_LOCAL0, "%s%s%s\n", strerr, ": ", strerror(errno));
   
    return;
}
void process_request(int d, struct sockaddr_in * cli_addr)
{
  char req_in[LREQUEST]; // for incoming requests
  char *message="Your IP address is: ";
  char response[LREQUEST];
  char *request="GET / HTTP/1.1";
  int n_bytes_read;
  char *response_err="<html><body><center><h1>400 Bad Request</h1></center></body></html>\n";
  char *opentags="<html><body><center><h1>";
  char *closetags="</center></body></html>";
  size_t len_err_response;
  char haddr[INET_ADDRSTRLEN];

  len_err_response=strlen(response_err);

  memset(req_in,0,LREQUEST);// filling by zeroes
  n_bytes_read=read(d,req_in,LREQUEST-1);

  if(n_bytes_read < 0)
  {
    WriteLogPError("Can't read from socket");
    if(errno == ECONNRESET) // connection reset
    {
        close(d);
        return;
    }
  }
  if(n_bytes_read == 0) //connection closed by remote
  {
      close(d);
      return;
  }
  else;
  if(!(strncasecmp(req_in,request,strlen(request))))
  {
//    inet_ntop(AF_INET,&cli_addr.sin_addr,haddr,LREQUEST);
    inet_ntop(AF_INET,&(cli_addr->sin_addr),haddr,INET_ADDRSTRLEN);
    snprintf(response,LREQUEST,"%s%s%s%s\n",opentags,message,haddr,closetags);
    if((write(d, response, strlen(response))) == -1)
        WriteLogPError("write()");
  }
  else
    if((write(d, response_err, len_err_response)) == -1)
        WriteLogPError("write()");

  return;
} 
int setnonblocking (int sfd)
{
  int flags;
  if((flags = fcntl (sfd, F_GETFL, 0)) == -1)
    {
      WriteLogPError("fcntl()");
      return -1;
    }

  flags |= O_NONBLOCK;
  if((fcntl (sfd, F_SETFL, flags)) == -1)
    {
      WriteLogPError("fcntl()");
      return -1;
    }

  return 0;
}

http_method_t find_http_method(const char *sval)
{
    http_method_t result = GET; /* value corresponding to etable[0] */
    int i = 0;
    for (i = 0; http_method[i] != NULL; ++i, ++result)
        if (!(strincmp(sval, http_method[i], 16))) 
          return result;
    return -1;
}
http_protocol_t find_http_protocol(const char *sval)
{
    http_protocol_t result = HTTP11; /* value corresponding to etable[0] */
    int i = 0;
    for (i = 0; http_protocol[i] != NULL; ++i, ++result)
        if (!(strncmp(sval, http_protocol[i], 16))) 
          return result;
    return -1;
}

int parse_http_request(http_request_t *req, const char *string_req)
{
   char *tmp;
   unsigned int i = 0;
/* search for request method */
   while(isblank(*(string_req + i))
        i++;
   tmp = string_req + i;
   while(!(isalpha(*(string_req + i))))
       i++;
  
  req->method=find_http_method(tmp);
 

/* search for params */
   while(string_req + i)
      i++;


/* search for http protocol */



    }






}

