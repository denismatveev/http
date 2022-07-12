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

#define REQUEST_URI_STRING_LENGTH 2048
#define REQUEST_LINE_LENGTH 8192
#define HTTP_HEADERS_MAX_SIZE 8192
#define INITIAL_DATA_SIZE (8+REQUEST_URI_STRING_LENGTH+9+HTTP_HEADERS_MAX_SIZE) //method+request-uri+http-version+headers
#define HEADERS_LIMIT 64 //total number of headers(general,request, entity, response headers)
#define MAX_EVENTS 10
#define SERVERNAME "Maya"
#define CONTENT_TYPE_MAX_LENGTH 16
#define HTTP_PROTOCOL_VERSION_MAX_LENGTH 9
#define REASON_CODE_NAME_MAX_LENGTH 21
#define HTTP_HEADER_NAME_MAX_LEN 21
#define HTTP_HEADER_VALUE_MAX_LEN 128
#define HTTP_METHOD_MAX_LEN 8
#define DATE_HEADER_MAX_LENGTH 64
#define HEADER_VALUE_MAX_LENGTH 512
#define STATUS_LINE_MAX_LENGTH (REASON_CODE_NAME_MAX_LENGTH + HTTP_PROTOCOL_VERSION_MAX_LENGTH + 3)
#define MAX_BODY_SIZE 1024*1024 // default is 1Mb

#define CRLF "\r\n"
#define SP " "
typedef unsigned char u_int_t;
/* here are enums aka X Macros */
#define HTTP_PROTOCOL(XX)            \
    XX(0,  HTTP10,      HTTP/1.0)    \
    XX(1,  HTTP11,      HTTP/1.1)    \

typedef enum http_protocol_version
{
#define XX(num, name, proto_str) PROTO_##name = num,
    HTTP_PROTOCOL(XX)
#undef XX
    INVALID_PROTO = -1
} http_protocol_version_t;


/* General Data structures */

#define HTTP_REASON_CODE(XX)                             \
    XX(200,  OK       ,        200 OK            )       \
    XX(400,  BAD_REQUEST,      400 Bad Request)          \
    XX(404,  NOT_FOUND,        404 Not Found)            \
    XX(413,  ENTITY_TOO_LARGE, 413 Entity Too Large)     \
    XX(500,  INTERNAL_ERROR,   500 Internal Error)       \
    XX(501,  NOT_IMPLEMENTED,  501 Not Implemented  )    \


typedef enum reason_code
{
#define XX(num, name, reason_str) REASON_##name = num,
    HTTP_REASON_CODE(XX)
#undef XX
}http_reason_code_t;

#define HTTP_CONTENT_TYPE(XX)                       \
XX(0, TEXT_HTML,            text/html)              \
XX(1, IMAGE_JPG,            image/jpg)              \
XX(2, APPLICATION_PDF,      application/pdf)        \
XX(3, IMAGE_PNG,            image/png)              \
XX(4, VIDEO_MPEG,           video/mpeg)             \
XX(5, TEXT_CSS,             text/css)               \

typedef enum content_type
{
#define XX(num, name, content_type_str) CONTENT_##name = num,
    HTTP_CONTENT_TYPE(XX)
#undef XX
    INVALID_MIME = -1
} http_content_type_t;

#define HTTP_ENTITY_HEADER_CONTENT_TYPE(XX)                           \
    XX(0, TEXT_HTML,            Content-Type: text/html)              \
    XX(1, IMAGE_JPG,            Content-Type: image/jpg)              \
    XX(2, APPLICATION_PDF,      Content-Type: application/pdf)        \
    XX(3, IMAGE_PNG,            Content-Type: image/png)              \
    XX(4, VIDEO_MPEG,           Content-Type: video/mpeg)             \
    XX(5, TEXT_CSS,             Content-Type: text/css)               \

typedef enum entity_content_type
{
#define XX(num, name, entity_content_type) ENTITY_HEADER_CONTENT_##name = num,
    HTTP_ENTITY_HEADER_CONTENT_TYPE(XX)
#undef XX
    INVALID_ENTITY_HEADER_MIME = -1
} http_entity_header_content_type_t;

#define HTTP_GENERAL_HEADER(XX)                       \
    XX(0, CACHE_CONTROL,      Cache-Control:)         \
    XX(1, CONNECTION,         Connection:)            \
    XX(2, DATE,               Date:)                  \
    XX(3, PRAGMA,             Pragma:)                \
    XX(4, TRAILER,            Trailer:)               \
    XX(5, TRANSFER_ENCODING,  Transfer-Encoding:)     \
    XX(6, UPGRADE,            Upgrade:)               \
    XX(7, VIA,                Via:)                   \
    XX(8, WARNING,            Warning:)               \

typedef enum general_header
{
#define XX(num, name, general_header_str) GENERAL_HEADER_##name = num,
    HTTP_GENERAL_HEADER(XX)
#undef XX
    INVALID_GENERAL_HEADER = -1
} http_general_header_t;


typedef enum __header_type
{
    http_response_header=0,
    http_general_header=1,
    http_request_header=2,
    http_entity_header=3,
    INVALID_HEADER_TYPE=-1
}http_header_type_t;

#define HTTP_ENTITY_HEADER(XX)                           \
    XX(0, ALLOW,                  Allow:)                \
    XX(1, CONTENT_ENCODING,       Content-Encoding:)     \
    XX(2, CONTENT_LANGUAGE,       Content-Language:)     \
    XX(3, CONTENT_LENGTH,         Content-Length:)       \
    XX(4, CONTENT_LOCATION,       Content-Location:)    \
    XX(5, CONTENT_MD5,            Content-MD5:)          \
    XX(6, CONTENT_RANGE,          Content-Range:)        \
    XX(7, CONTENT_TYPE,           Content-Type:)        \
    XX(8, EXPIRES,                Expires:)              \
    XX(9, LAST_MODIFIED,          Last-Modified:)        \

typedef enum enity_header
{
#define XX(num, name, entity_header_str) ENTITY_HEADER_##name = num,
    HTTP_ENTITY_HEADER(XX)
#undef XX
    INVALID_ENTITY_HEADER = -1
} http_entity_header_t;

/* The structure below is used both in request and response */

typedef struct __http_header_node_t
{
    int http_header; // enum like Host:
    char http_header_value[HTTP_HEADER_VALUE_MAX_LEN]; // name like ya.ru
    http_header_type_t type;
    struct __http_header_node_t* next;
}http_header_node_t;

typedef struct __http_headers_list
{
    http_header_node_t* first;
    size_t limit; // max number of headers
    size_t capacity;

}http_headers_list_t;


/* Request related data structures */

#define HTTP_METHOD(XX)                   \
    XX(0, GET,                  GET)      \
    XX(1, POST,                 POST)     \
    XX(2, HEAD,                 HEAD)     \
    XX(3, PUT,                  PUT)      \
    XX(4, DELETE,               DELETE)   \
    XX(5, CONNECT,              CONNECT)  \
    XX(6, OPTIONS,              OPTIONS)  \
    XX(7, TRACE,                TRACE)    \
    XX(8, PATCH,                PATCH)    \

typedef enum http_method
{
#define XX(num, name, http_method_str) HTTP_METHOD_##name = num,
    HTTP_METHOD(XX)
#undef XX
    INVALID_METHOD = -1
} http_method_t;

#define HTTP_REQUEST_HEADER(XX)                                  \
    XX(0, ACCEPT,                         Accept:)               \
    XX(1, ACCEPT_CHARSET,                 Accept-Charset:)       \
    XX(2, ACCEPT_ENCODING,                Accept-Encoding:)      \
    XX(3, ACCEPT_LANGUAGE,                Accept-Language:)      \
    XX(4, AUTHORIZATION,                  Authorization:)        \
    XX(5, EXPECT,                         Expect:)               \
    XX(6, FROM,                           From:)                 \
    XX(7, HOST,                           Host:)                 \
    XX(8, IF_MATCH,                       If-Match:)              \
    XX(9, IF_MODIFIED_SINCE,              If-Modified-Since:)    \
    XX(10, IF_NONE_MATCH,                 If-None-Match:)        \
    XX(11, IF_RANGE,                      If-Range:)             \
    XX(12, IF_UNMODIFIED_SINCE,           If-Unmodified-Since:)  \
    XX(13, MAX_FORWARDS,                  Max-Forwards:)         \
    XX(14, PROXY_AUTHORIZATION,           Proxy-Authorization:)  \
    XX(15, RANGE,                         Range:)                \
    XX(16, REFERER,                       Referer:)              \
    XX(17, TE,                            TE:)                   \
    XX(18, USER_AGENT,                    User-Agent:)           \

typedef enum request_header
{
#define XX(num, name, http_request_header_str) REQUEST_HEADER_##name = num,
    HTTP_REQUEST_HEADER(XX)
#undef XX
    INVALID_REQUEST_HEADER = -1
} http_request_header_t;


typedef struct __http_request_line
{
    http_method_t method;
    http_protocol_version_t http_version;
    char request_URI[REQUEST_URI_STRING_LENGTH];
}http_request_line_t;

typedef struct __http_request
{
    http_request_line_t req_line;
    http_headers_list_t* headers;
    unsigned char parsing_result;
    char* message_body[MAX_BODY_SIZE];//TODO find out in https://tools.ietf.org/html/rfc2616 size of message_body. message_body should not always be sent in request if that request does not imply this
    //(GET should not be sent message_body)
} http_request_t;


/* Response related data structures */

#define HTTP_RESPONSE_HEADER(XX)                       \
    XX(0, ACCEPT_RANGES,      Accept-Ranges:)          \
    XX(1, AGE,                Age:)                    \
    XX(2, ETAG,               ETag:)                   \
    XX(3, LOCATION,           Location:)               \
    XX(4, PROXY_AUTHENTICATE, Proxy-Authenticate:)     \
    XX(5, RETRY_AFTER,        Retry-After:)            \
    XX(6, SERVER,             Server:)                 \
    XX(7, VARY,               Vary:)                   \
    XX(8, WWW_AUTHENTICATE,   WWW-Authenticate:)       \

typedef enum response_header
{
#define XX(num, name, response_header_str) RESPONSE_HEADER_##name = num,
    HTTP_RESPONSE_HEADER(XX)
#undef XX
    INVALID_RESPONSE_HEADER = -1
} http_response_header_t;


typedef struct __http_status_line
{
    http_protocol_version_t hv;
    http_reason_code_t rc;
} http_status_line_t;

typedef struct __http_response
{
    http_status_line_t sl;
    http_headers_list_t* headers;
    char message_body[MAX_BODY_SIZE];

}http_response_t;

/* Functions */
/*
 * GET /query.php?firstname=denis&lastname=matveev HTTP/1.1 \r\n Host:denismatveev.me \r\n
 * The result of stage 1:

 *
 */

/* Request related functions */
/* Interface */

http_request_t* init_http_request(void);
void delete_http_request(http_request_t*);
int process_http_data(http_request_t*, char *);//converts string -> http_request_t
/* inner functions */
http_header_node_t* init_http_request_header_node(const char http_header_name[], const char http_header_value[]);
http_header_node_t* init_http_response_header_node(const char http_header_name[], const char http_header_value[]);
void destroy_http_header_node(http_header_node_t*);
http_headers_list_t* init_http_headers_list(http_header_node_t *first_node);
void destroy_http_headers_list(http_headers_list_t*);
int create_http_request(http_request_t* request, http_headers_list_t *list, http_request_line_t req_line);
int push_http_header_to_list(http_headers_list_t* list, http_header_node_t* header);
http_general_header_t str_to_http_general_header(const char *);
http_entity_header_t str_to_http_entity_header(const char *);
http_response_header_t str_to_http_response_header(const char *);
http_request_header_t str_to_http_request_header(const char *h);
int parse_http_header_line(char* header_line, char *header, char *value);
// parses Request-Line
// input: char*
// output: http_request_line
// returns 0 if OK, 1 if failed
int parse_request_line(http_request_line_t*, char *);
// gets system date and time in GMT
// output: char*, max 128 chars
// returns 0 if OK, 1 if failed
int get_current_date_str(char*);
http_method_t str_to_http_method(const char *);
http_protocol_version_t str_to_http_protocol(const char *);

/* Response related functions */
/* Interface */
http_response_t* init_http_response(void);
void delete_http_response(http_response_t*);
int create_http_response(http_response_t* response, http_headers_list_t *list, http_status_line_t status);
int process_http_response(char* response, http_response_t *rs, size_t str_len);// converts http_response_t -> string
int set_reason_code(http_response_t* r, int code);
int add_header_to_response(http_response_t* resp, char* header_name, char *header_value);

/* inner functions */

// serialize response to be sent
// input: http_response_t* r
// output: char* serialized_response
// returns 0 if OK, 1 if failed
long get_file_size(int fd);
// file content type by its extension. Max extension length is 5
// input: char* filename
// output: content type http_content_type_t
// returns http_content_type_t
http_entity_header_content_type_t get_file_MIME_type(const char *filename);
// gets content file type in c string by filename
// input: char* filename
// output: char* content type
// returns 0 if OK, 1 if failed

int file_MIME_type_to_str(char* content_type, const char* filename, unsigned char type_len);
// creates status line
// input http_protocol_version_t, http_reason_code_t
// output char*, buffer must be at least STATUS_LINE_MAX_LENGTH length
int status_line_to_str(char*, http_protocol_version_t, http_reason_code_t, unsigned char str_len);

int http_ptorocol_code_to_str(char *str, http_protocol_version_t rt, unsigned char str_len);
int reason_code_to_str(char *, http_reason_code_t, unsigned char len);
int http_method_to_str(http_method_t h, char* str, unsigned char str_len);

int content_type_to_str(char *str, http_entity_header_content_type_t ct, unsigned char str_len);
int http_general_header_to_str(http_general_header_t h, char* str, unsigned char str_len);
int http_request_header_to_str(http_request_header_t h, char* str, unsigned char str_len);
int http_entity_header_to_str(http_entity_header_t h, char* str, unsigned char str_len);
int http_response_header_to_str(http_response_header_t h, char* str, unsigned char str_len);

int header_name_to_str_value_by_type(const http_header_node_t*, char [], char []);
int validation_content_type(const char* ct);
int add_file_as_message_body(http_response_t * response, int fd, const char* filename);
#endif /*_HTTP_H*/
