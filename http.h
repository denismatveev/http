#ifndef _HTTP_H
#define _HTTP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>

#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "common.h"
#define PARAMS_STRING_LENGTH 131072

extern char *http_method[], *http_protocol_version[];
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
    HTTP11 = 0,
    HTTP2
} http_protocol_version_t;
// I need to implement only 200, 400 and 404 responses
typedef enum reason_code
{
  Bad_Request = 400,
  Not_found = 404,
  OK = 200
}reason_code_t;

typedef enum response_header
{
  Age = 0,
  Location,
  Proxy_Authenticate,
  Public,
  Retry_After,
  Server,
  Vary,
  Warning,
  WWW_Authenticate
}response_header_t;

typedef struct __response
{
  char *status_line;
  char *response_header;
  int message_body;//Most likely this should be a http file, descriptor int
}response;


int create_status_line(char*, const http_protocol_version_t, const reason_code_t);
int create_response_header(char*, const response_header_t, const char *);
int create_response(char*, char*, int);

//see https://rfc2.ru/2068.rfc/30#p6
//Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF

http_method_t find_http_method(const char *);
http_protocol_version_t find_http_protocol_version(const char *);

void process_request(int, struct sockaddr_in*);

typedef struct __http_request
{
    http_method_t method;  
    http_protocol_version_t http_proto;
    char params[PARAMS_STRING_LENGTH];//128 kb maximum, it is filename
} http_request_t;
int fill_http_request(http_request_t *, const char *);
#endif /*_HTTP_H*/
