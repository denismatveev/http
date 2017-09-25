#ifndef _HTTPD_
#define _HTTPD_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/stat.h>
/* enumeration of all available HTTP methods */
typedef enum 
{
    GET = 0,
    POST,
    HEAD,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH
} http_method_t;
/* array of strings to search symbol name of HTTP method */
char **http_method = {"GET", "POST", "HEAD","PUT", "DELETE", "CONNECT", "OPTIONS","TRACE", "PATCH", NULL};
typedef enum
{
    HTTP11 = 1,
    HTTP20
} http_protocol_t;

char **http_protocol = {"HTTP/1.1", "HTTP/2.0", NULL};
http_method_t find_http_method(const char *sval);
http_protocol_t find http_protocol(const char *sval);
int setnonblocking (int);
void process_request(int, struct sockaddr_in*);
void WriteLog(const char *format, ...);
void WriteLogPError(const char*);
typedef stuct __http_request
{
    char host[256];//max length of domain name
    http_method_t method;  
    http_protocol_t http_proto;
    char params[131072];//128 kb maximum
} http_request;
typedef http_request http_request_t;
int parse_http_request(http_request_t, const char *);
#endif /*_HTTPD_*/
