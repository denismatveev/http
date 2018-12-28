#ifndef HTTP_H
#define HTTP_H
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
#define HOST_STRING_LENGTH 8192
#define INITIAL_DATA_SIZE 132000 //method+params+proto+hostname
#define HEADERS_LIMIT 64

extern char *http_method[], *http_protocol_version[], *reason_code_name[], *content_type[], *file_ext[],*general_header[], *response_header[], *entity_header[], *request_header[];
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
  PATCH,
  INVALID_METHOD = -1
} http_method_t;

typedef enum http_protocol_version 
{
  HTTP10 = 0,
  HTTP11,
  HTTP2,
  INVALID_PROTO = -1
} http_protocol_version_t;

typedef enum reason_code
{
  Bad_Request = 400,
  Not_found = 404,
  Entity_Too_Large = 413,
  OK = 200,
  Internal_Error = 500,
  Not_implemented = 501
}http_reason_code_t;

typedef enum content_type
{
  text_html = 0,
  image_jpg,
  application_pdf,
  image_png,
  video_mpeg,
  text_css,
  INVALID_MIME = -1
}http_content_type_t;

typedef struct http_response_header
{
  char status_line[256];
  char server[256];
  char date[256];
  char content_type[256];
  long content_length_num;
  char content_length[256];
}http_response_header_old_t;

typedef struct __response
{
  http_response_header_old_t header;
  int message_body;// file descriptor int
}http_response_old_t;


int create_status_line(char*, size_t, http_protocol_version_t, http_reason_code_t);
http_response_old_t* create_http_response(void);
void fill_http_response(http_response_old_t *resp, const char* status_line);
void delete_http_response(http_response_old_t*);
int http_ptorocol_code_to_str(char *str, size_t len, http_protocol_version_t rt);
int reason_code_to_str(char *, size_t, http_reason_code_t);

http_method_t find_http_method(const char *);
http_protocol_version_t find_http_protocol_version(const char *);

typedef enum __response_header
{
  Accept_Ranges = 0,
  Age,
  ETag,
  Location,
  Proxy_Authenticate,
  Retry_After,
  Server,
  Vary,
  WWW_Authenticate

}http_response_header_t;

typedef enum __general_header
{
  Cache_Control = 0,
  Connection,
  Date,
  Pragma,
  Trailer,
  Transfer_Encoding,
  Upgrade,
  Via,
  Warning

}http_general_header_t;

typedef enum __request_header
{
  Accept = 0,
  Accept_Charset,
  Accept_Encoding,
  Accept_Language,
  Authorization,
  Expect,
  From,
  Host,
  If_Match,
  If_Modified_Since,
  If_None_Match,
  If_Range,
  If_Unmodified_Since,
  Max_Forwards,
  Proxy_Authorization,
  Range,
  Referer,
  TE,
  User_Agent

}http_request_header_t;
typedef enum __entity_header
{

  Allow = 0,
  Content_Encoding,
  Content_Language,
  Content_Length,
  Content_Location,
  Content_MD5,
  Content_Range,
  Content_Type,
  Expires,
  Last_Modified

}http_entity_header_t;
typedef struct __http_request_line
{
  http_method_t method;
  http_protocol_version_t http_proto;
  char request_URI[PARAMS_STRING_LENGTH];
}http_request_line_t;

typedef struct  __headers
{
  http_general_header_t gh[HEADERS_LIMIT];
  http_request_header_t reqh[HEADERS_LIMIT];
  http_response_header_t resph[HEADERS_LIMIT];
  http_entity_header_t eh[HEADERS_LIMIT];

}http_headers_t;

typedef struct __http_request
{
  http_request_line_t req_line;
  http_headers_t headers;
} http_request_t;

typedef struct __http_status_line
{
  http_protocol_version_t hv;
  http_reason_code_t rc;
} http_status_line_t;

typedef struct __http_response
{
  http_status_line_t sl;
  http_headers_t headers;

}http_response_t;

typedef struct __http_request_old
{
  http_method_t method;
  http_protocol_version_t http_proto;
  char host[HOST_STRING_LENGTH];// in case of Host header length exceeded limit, return "413 Entity Too Large"
  int port;
  char params[PARAMS_STRING_LENGTH];//128 kb maximum, it is a filename

} http_request_old_t;

http_request_old_t* create_request(void);
void delete_http_request(http_request_old_t*);

typedef struct __client_data
{
  char initial_data[INITIAL_DATA_SIZE];
  int client_socket;
} raw_client_data_t;

raw_client_data_t* create_raw_data(void);
void delete_raw_data(raw_client_data_t*);
int create_http_request_from_raw_data(http_request_old_t *ht, const raw_client_data_t *rd);
int get_current_date_string(char* date, size_t n);
int create_date_header(char* dheader, size_t n);
int create_server_header(char* server_name, size_t len);

int create_200_response(http_response_old_t*,const http_request_old_t*);
int create_404_response(http_response_old_t*,const http_request_old_t*);
int create_501_response(http_response_old_t*,const http_request_old_t*);
int create_500_response(http_response_old_t*,const http_request_old_t*);
int create_400_response(http_response_old_t*,const http_request_old_t*);
int create_413_response(http_response_old_t*,const http_request_old_t*);
size_t create_serialized_http_header(char* serialized, const http_response_header_old_t* rs, size_t size);
int convert_Content_Length(http_response_header_old_t* header);
long get_file_size(int fd);
http_content_type_t get_file_MIME_type(const char *filename);
int get_file_MIME_type_in_str(char* content_type, size_t len, const char* filename);
#endif /*_HTTP_H*/
