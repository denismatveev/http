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
#define PARAMS_STRING_LENGTH 131072

/* enumeration of all available HTTP methods */
typedef enum http_method
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
typedef enum http_protocol_version 
{
    HTTP11 = 1,
    HTTP2
} http_protocol_version_t;

http_method_t find_http_method(const char *sval);
http_protocol_version_t find_http_protocol_version(const char *sval);
int setnonblocking (int);
void process_request(int, struct sockaddr_in*);
void WriteLog(const char *format, ...);
void WriteLogPError(const char*);
typedef struct __http_request
{
    char host[256];//max length of domain name
    http_method_t method;  
    http_protocol_version_t http_proto;
    char params[PARAMS_STRING_LENGTH];//128 kb maximum
} http_request_t;
int fill_http_request(http_request_t *, const char *);
#endif /*_HTTPD_*/
