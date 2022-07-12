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
//#include "common.h"

#define REQUEST_URI_STRING_LENGTH 2048
#define REQUEST_LINE_LENGTH 8192
#define HTTP_HEADERS_MAX_SIZE 8192
#define INITIAL_DATA_SIZE (8+REQUEST_URI_STRING_LENGTH+9+HTTP_HEADERS_MAX_SIZE) //method+request-uri+http-version+headers
#define HEADERS_LIMIT 64 //total number of headers(general,request, entity, response headers)
#define MAX_EVENTS 10
#define servername "Maya web server"
#define CONTENT_TYPE_MAX_LENGTH 16
#define HTTP_PROTOCOL_VERSION_MAX_LENGTH 9
#define REASON_CODE_NAME_MAX_LENGTH 21
#define HTTP_HEADER_NAME_MAX_LEN 21
#define HTTP_HEADER_VALUE_MAX_LEN 128
#define HTTP_METHOD_MAX_LEN 8
#define STATUS_LINE_MAX_LENGTH (REASON_CODE_NAME_MAX_LENGTH + HTTP_PROTOCOL_VERSION_MAX_LENGTH + 3)

#define CRLF "\r\n"
#define SP " "
// the functions below work only on little endian
#define str3cmp(m, c0, c1, c2, c3)                                            \
    m[0] == c0 && m[1] == c1 && m[2] == c2

#define str4cmp(m, c0, c1, c2, c3)                                            \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3

#define str5cmp(m, c0, c1, c2, c3, c4)                                        \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 && m[4] == c4

#define str6cmp(m, c0, c1, c2, c3, c4, c5)                                    \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3                      \
        && m[4] == c4 && m[5] == c5

#define str7_cmp(m, c0, c1, c2, c3, c4, c5, c6, c7)                           \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3                      \
        && m[4] == c4 && m[5] == c5 && m[6] == c6

#define str8cmp(m, c0, c1, c2, c3, c4, c5, c6, c7)                            \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3                      \
        && m[4] == c4 && m[5] == c5 && m[6] == c6 && m[7] == c7

#define str9cmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8)                        \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3                      \
        && m[4] == c4 && m[5] == c5 && m[6] == c6 && m[7] == c7 && m[8] == c8


#define HTTP_PROTOCOL(XX)           \
  XX(0,  HTTP10,      HTTP/1.0)     \
  XX(1,  HTTP11,      HTTP/1.1)     \

typedef enum http_protocol_version
{
  #define XX(num, name, proto_str) PROTO_##name = num,
    HTTP_PROTOCOL(XX)
  #undef XX
    INVALID_PROTO=-1
} http_protocol_version_t;


typedef unsigned char u_int_t;

// An example of request string
// GET /query.php?firstname=denis&lastname=matveev HTTP/1.1 \r\n Host:denismatveev.me \r\n

/* General Data structures */

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

#define HTTP_REASON_CODE(XX)                           \
  XX(400,  BAD_REQUEST,      400 Bad Request)          \
  XX(404,  NOT_FOUND,        404 Not Found)            \
  XX(413,  ENTITY_TOO_LARGE, 413 Entity Too Large)     \
  XX(200,  OK       ,        200 OK            )       \
  XX(500,  INTERNAL_ERROR,   500 Internal Error)       \
  XX(501,  NOT_IMPLEMENTED,  501 Not Implemented  )    \


typedef enum reason_code
{
#define XX(num, name, proto_str) REASON_##name = num,
  HTTP_REASON_CODE(XX)
#undef XX
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
}http_entity_header_content_type_t;

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
    Warning,
    INVALID_GENERAL_HEADER=-1

}http_general_header_t;

typedef enum __header_type
{
    http_response_header=0,
    http_general_header=1,
    http_request_header=2,
    http_entity_header=3,
    INVALID_HEADER_TYPE=-1
}http_header_type_t;


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
    Last_Modified,
    INVALID_ENTITY_HEADER=-1

}http_entity_header_t;

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
    User_Agent,
    INVALID_REQUEST_HEADER=-1

}http_request_header_t;

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
    //char* message_body[1024];//TODO find out in https://tools.ietf.org/html/rfc2616 size of message_body. message_body should not always be sent in request if that request does not imply this
    //(GET should not be sent message_body)
} http_request_t;


/* Response related data structures */

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
    WWW_Authenticate,
    INVALID_RESPONSE_HEADER=-1

}http_response_header_t;

typedef struct __http_status_line
{
    http_protocol_version_t hv;
    http_reason_code_t rc;
} http_status_line_t;

// Response to be sent is  in function
typedef struct __http_response
{
    http_status_line_t sl;
    http_headers_list_t* headers;
    //int fd; // file descriptor to be sent(in case of error page or requesting a file) // TODO this is not a suitable place for storing socket in http parsing library. Move to jobs

}http_response_t;

/* Functions */

/* General functions */
/* str_to() family - transform header numeric representation into word to be sent as a response
 * create() family - fill in data structures with passed data
 * init() and destroy family - allocate memory for data structures and delete it
 * parse() family - parse string from http request
 * process()
 */
/*
 *
 * GET /query.php?firstname=denis&lastname=matveev HTTP/1.1 \r\n Host:denismatveev.me \r\n
 * The result of stage 1:

 *
 */
http_header_node_t* init_http_request_header_node(const char http_header_name[], const char http_header_value[]);
http_header_node_t* init_http_response_header_node(const char http_header_name[], const char http_header_value[]);
void destroy_http_header_node(http_header_node_t*);
http_headers_list_t* init_http_headers_list(http_header_node_t *first_node);
void destroy_http_headers_list(http_headers_list_t*);
int push_http_header_to_list(http_headers_list_t* list, http_header_node_t* header);
http_general_header_t str_to_http_general_header(const char *);
http_entity_header_t str_to_http_entity_header(const char *);
http_response_header_t str_to_http_response_header(const char *);
http_request_header_t str_to_http_request_header(const char *h);
int header_name_to_str_value_by_type(const http_header_node_t*, char [], char []);


/* Request related functions */

int create_http_request(http_request_t* request, http_headers_list_t *list, http_request_line_t req_line);

int parse_http_header_line(char* header_line, char *header, char *value);

// process client raw data and put into request structure
// input: char*
// output: http_request_t*
// returns 0 if OK, 1 if failed
int process_http_data(http_request_t*, char *);
http_request_t* init_http_request(void);
void delete_http_request(http_request_t*);

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

int create_http_response(http_response_t* response, http_headers_list_t *list, http_status_line_t status);
// creates header Server:
// output char*, max length is len
int create_server_header(char* server_name, size_t len);
// creates Date: header in user's locale
// output: char*, max 128 chars
// returns 0 if OK, 1 if failed
int create_date_header_to_str(char*, unsigned char);
// creates status line with error
// input: http_reason_code_t code, char* error_file
// output: http_response_t* r
// returns 0 if OK, 1 if failed
int create_error_response(http_response_t* r, const http_reason_code_t code, char* error_file);
// serialize response to be sent
// input: http_response_t* r
// output: char* serialized_response
// returns 0 if OK, 1 if failed
int response_to_str(char* serialized_response, const http_response_t* r);
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

int get_file_MIME_type_in_str(char* content_type, const char* filename);

// creates status line
// input http_protocol_version_t, http_reason_code_t
// output char*, buffer must be at least STATUS_LINE_MAX_LENGTH length
int status_line_to_str(char*, http_protocol_version_t, http_reason_code_t, unsigned char str_len);

int http_ptorocol_code_to_str(char *str, http_protocol_version_t rt, unsigned char str_len);
int reason_code_to_str(char *, http_reason_code_t, unsigned char len);
int http_method_to_str(http_method_t h, char* str, unsigned char str_len);




#endif /*_HTTP_H*/
