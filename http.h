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
#define servername "Maya"
#define CONTENT_TYPE_MAX_LENGTH 16
#define HTTP_PROTOCOL_VERSION_MAX_LENGTH 7
#define REASON_CODE_NAME_MAX_LENGTH 21
#define HTTP_HEADER_NAME_MAX_LEN 21
#define HTTP_HEADER_VALUE_MAX_LEN 128
#define HTTP_METHOD_MAX_LEN 8
#define STATUS_LINE_MAX_LENGTH (REASON_CODE_NAME_MAX_LENGTH + HTTP_PROTOCOL_VERSION_MAX_LENGTH + 3)
#define TOLOWERCASE(c)      (char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)
#define TOUPPERCASE(c)      (char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)
#define CRLF "\r\n"
#define SP " "

typedef char u_int_t;

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
typedef enum __header_type
{
    http_response_header=0,
    http_general_header=1,
    http_request_header=2,
    http_entity_header=3,
    INVALID_HEADER_TYPE=-1
}http_header_type_t;
typedef enum __state_parsing
{
    STATE_INVALID_METHOD=1,
    STATE_INVALID_PROTO,
    STATE_INVALID_MIME,
    STATE_INVALID_RESPONSE_HEADER,
    STATE_INVALID_GENERAL_HEADER,
    STATE_INVALID_REQUEST_HEADER,
    STATE_INVALID_ENTITY_HEADER

}state_parsing_t;
typedef struct __http_request_line
{
    http_method_t method;
    http_protocol_version_t http_version;
    char request_URI[REQUEST_URI_STRING_LENGTH];
}http_request_line_t;

typedef struct __http_header_node_t
{
    int http_header; // enum like Host:
    char http_header_value[HTTP_HEADER_VALUE_MAX_LEN]; // name like ya.ru
    http_header_type_t type;
    state_parsing_t state;
    struct __http_header_node_t* next;
}http_header_node_t;

typedef struct __http_headers_list
{
    http_header_node_t* first;
    size_t limit; // max number of headers
    size_t capacity;

}http_headers_list_t;


http_header_node_t* init_http_request_header_node(const char http_header_name[HTTP_HEADER_NAME_MAX_LEN], const char http_header_value[HTTP_HEADER_VALUE_MAX_LEN]);
http_header_node_t* init_http_response_header_node(const char http_header_name[HTTP_HEADER_NAME_MAX_LEN], const char http_header_value[HTTP_HEADER_VALUE_MAX_LEN]);
void destroy_http_headers_node(http_header_node_t*);
http_headers_list_t* init_http_headers_list(const http_header_node_t* first_node);
void destroy_http_headers_list(http_headers_list_t*);
int add_http_header_to_list(http_headers_list_t* list, const http_header_node_t* header);

typedef struct __http_request
{
    http_request_line_t req_line;
    http_headers_list_t* headers;
    char parse_result; // indicates if parsing was successful or not, useful for 'BAD REQUEST'
    //char* message_body[1024];//TODO find out in https://tools.ietf.org/html/rfc2616 size of message_body!
} http_request_t;

typedef struct __http_status_line
{
    http_protocol_version_t hv;
    http_reason_code_t rc;
} http_status_line_t;

typedef struct __http_response
{
    http_status_line_t sl;
    http_headers_list_t* headers;
    //char* response; // used in case cgi, proxy or similar responses. TODO Move to jobs
    //int fd; // file descriptor to be sent(in case of error page or requesting a file) // TODO this is not a suitable place for storing socket in http parsing library. Move to jobs

}http_response_t;

int create_http_request(http_request_t* request, const http_headers_list_t* list, http_request_line_t req_line);
int create_http_response(http_response_t* response, http_headers_list_t *list, http_status_line_t status);
typedef struct __client_data
{
    char initial_data[INITIAL_DATA_SIZE];
    int client_socket; // TODO this is not a suitable place for storing socket in http parsing library. Move to jobs
} raw_client_data_t;

//raw_client_data_t* create_raw_data(void);
//void delete_raw_data(raw_client_data_t*);
int set_raw_data(raw_client_data_t*, const char*);
// parses client raw data and put into request structure
// input: raw_client_data_t*
// output: http_request_t*
// returns 0 if OK, 1 if failed
int parse_raw_data(http_request_t*, raw_client_data_t *);
// parses Request-Line
// input: char*
// output: http_request_line
// returns 0 if OK, 1 is failed
int parse_request_line(http_request_line_t*, const char*);
// gets system date and time in GMT
// output: char*, max 128 chars
// returns 0 if OK, 1 is failed
int get_current_date_string(char*);

// creates Date: header in user's locale
// output: char*, max 128 chars
// returns 0 if OK, 1 is failed
int create_date_header(char*);

// creates header Server:
// output char*, max length is len
int create_server_header(char* server_name, size_t len);

// creates status line with error
// input: http_reason_code_t code, char* error_file
// output: http_response_t* r
// returns 0 if OK, 1 is failed
int create_error_response(http_response_t* r, const http_reason_code_t code, char* error_file);

// serialize response to be sent
// input: http_response_t* r
// output: char* serialized_response
// returns 0 if OK, 1 is failed
int serialize_response(char* serialized_response, const http_response_t* r);

long get_file_size(int fd);
// file content type by its extension. Max extension length is 5
// input: char* filename
// output: content type http_content_type_t
// returns http_content_type_t

http_content_type_t get_file_MIME_type(const char *filename);
// gets content file type in c string by filename
// input: char* filename
// output: char* content type
// returns 0 if OK, 1 is failed
int get_file_MIME_type_in_str(char* content_type, const char* filename);

// creates status line
// input http_protocol_version_t, http_reason_code_t
// output char*, buffer must be at least STATUS_LINE_MAX_LENGTH length
int create_status_line(char*, http_protocol_version_t, http_reason_code_t);
int http_ptorocol_code_to_str(char *str, http_protocol_version_t rt);
int reason_code_to_str(char *, http_reason_code_t);
int parse_http_header_line(char* header_line, char *header, char *value);
http_method_t find_http_method(const char *);
http_protocol_version_t find_http_protocol_version(const char *);
#endif /*_HTTP_H*/
