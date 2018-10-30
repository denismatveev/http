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
#define INITIAL_DATA_SIZE 132000 //method+params+proto+hostname

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
}http_reason_code_t;


typedef struct http_response_header
{
  char* status_line;
  char* server;
  char* date;
  char* content_type;
  size_t content_length;
}http_response_header_t;

typedef struct __response
{
  http_response_header_t header;
  int message_body;//Most likely this should be a http file, descriptor int
}http_response_t;


int create_status_line(char*, size_t, http_protocol_version_t, http_reason_code_t);
http_response_t* create_response(const char*);
int delete_http_response(http_response_t*);
int reason_code_to_str(char *, size_t, http_reason_code_t);
//see https://rfc2.ru/2068.rfc/30#p6
//Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF

http_method_t find_http_method(const char *);
http_protocol_version_t find_http_protocol_version(const char *);


typedef struct __http_request
{
  http_method_t method;
  http_protocol_version_t http_proto;
  char params[PARAMS_STRING_LENGTH];//128 kb maximum, it is a filename

} http_request_t;

http_request_t* create_request();
void delete_request(http_request_t*);

typedef struct __client_data
{
  char initial_data[INITIAL_DATA_SIZE];
  int client_socket;
} raw_client_data_t;

raw_client_data_t* create_raw_data();
void delete_raw_data(raw_client_data_t*);
int http_request_from_raw_data(http_request_t *ht, const raw_client_data_t *rd);
int run_request(http_response_t* resp, const http_request_t *ht, int client_sock);
#endif /*_HTTP_H*/
