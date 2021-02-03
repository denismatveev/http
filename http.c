#include "http.h"
#include <locale.h>
#include <time.h>

/* General */

char *http_method[] = {
    "GET",
    "POST",
    "HEAD",
    "PUT",
    "DELETE",
    "CONNECT",
    "OPTIONS",
    "TRACE",
    "PATCH",
    NULL
};
char *http_protocol_version[] = {
    "HTTP/1.0",
    "HTTP/1.1",
    "HTTP/2",
    NULL
};
char *reason_code_name[] = {
    "400 Bad Request",
    "404 Not Found",
    "200 OK",
    "413 Entity Too Large",
    "500 Internal Error",
    "501 Not Implemented",
    NULL
};
char *content_type[] = {
    "text/html",
    "image/jpg",
    "application/pdf",
    "image/png",
    "video/mpeg",
    "text/css",
    NULL
};
char *file_ext[] = {".html", ".jpg", ".pdf", ".png", ".mpeg", ".css", NULL};
// the following is used in both request and response
char* general_header[] = {
    "Cache-Control:",
    "Connection:",
    "Date:",
    "Pragma:",
    "Trailer:",
    "Transfer-Encoding:",
    "Upgrade:",
    "Via:",
    "Warning:",
    NULL
};
// the following is used in response and therefore contains colon and space
char* response_header[] = {
    "Accept-Ranges: ",
    "Age: ",
    "ETag: ",
    "Location: ",
    "Proxy-Authenticate: ",
    "Retry-After: ",
    "Server: ",
    "Vary: ",
    "WWW-Authenticate: ",
    NULL
};
// the following is used in both request and response
char* entity_header[] = {

    "Allow:",
    "Content-Encoding:",
    "Content-Language:",
    "Content-Length:",
    "Content-Location:",
    "Content-MD5:",
    "Content-Range:",
    "Content-Type:",
    "Expires:",
    "Last-Modified:",
    NULL
};

char *request_header[] = {
    "Accept: ",
    "Accept-Charset:",
    "Accept-Encoding:",
    "Accept-Language:",
    "Authorization:",
    "Expect:",
    "From:",
    "Host:",
    "If-Match: ",
    "If-Modified-Since: ",
    "If-None-Match: ",
    "If-Range:",
    "If-Unmodified-Since:",
    "Max-Forwards:",
    "Proxy-Authorization:",
    "Range:",
    "Referer:",
    "TE:",
    "User-Agent:",
    NULL
};

http_method_t find_http_method(const char *sval)
{
    http_method_t result = GET; /* value corresponding to etable[0] */
    int i = 0;
    for (i = 0; http_method[i] != NULL; ++i, ++result)
        if (!(strncmp(sval, http_method[i], 16)))
            return result;
    return INVALID_METHOD;
}
http_protocol_version_t find_http_protocol_version(const char *sval)
{
    http_protocol_version_t result = HTTP10; /* value corresponding to etable[0] */
    int i = 0;
    for (i = 0; http_protocol_version[i] != NULL; ++i, ++result)
        if (!(strncmp(sval, http_protocol_version[i], 16)))
            return result;
    return INVALID_PROTO;
}

// convert content_type into string
// It is for supported MIMEs, you must ajust CONTENT_TYPE_MAX_LENGTH for new implemented types

int content_type_to_str(char *str, http_content_type_t ct)
{
    size_t len = CONTENT_TYPE_MAX_LENGTH;
    size_t content_type_length;
    switch (ct)
    {
    case text_html:
        content_type_length=strlen(content_type[0]);
        if(len <= content_type_length)
            return 1;
        strcpy(str,content_type[0]);
        break;
    case image_jpg:
        content_type_length=strlen(content_type[1]);
        if(len <= content_type_length)
            return 2;
        strcpy(str,content_type[1]);
        break;
    case application_pdf:
        content_type_length=strlen(content_type[2]);
        if(len <= content_type_length)
            return 3;
        strcpy(str,content_type[2]);
        break;
    case image_png:
        content_type_length=strlen(content_type[3]);
        if(len <= content_type_length)
            return 4;
        strcpy(str,content_type[3]);
        break;
    case video_mpeg:
        content_type_length=strlen(content_type[4]);
        if(len <= content_type_length)
            return 5;
        strcpy(str,content_type[4]);
        break;
    case text_css:
        content_type_length=strlen(content_type[5]);
        if(len <= content_type_length)
            return 6;
        strcpy(str,content_type[5]);
        break;
    case INVALID_MIME:
        return -1;
    }
    return 0;
}
// converts http code enumeration into c string
int http_ptorocol_code_to_str(char *str, http_protocol_version_t rt)
{
    size_t len = HTTP_PROTOCOL_VERSION_MAX_LENGTH;
    size_t protocol_size;
    switch(rt)
    {
    case HTTP10:
        protocol_size=strlen(http_protocol_version[0]);
        if(len <= protocol_size)
            return 1;
        strcpy(str, http_protocol_version[0]);
        break;
    case HTTP11:
        protocol_size=strlen(http_protocol_version[0]);
        if(len <= protocol_size)
            return 1;
        strcpy(str, http_protocol_version[0]);
        break;
    case HTTP2:
        protocol_size=strlen(http_protocol_version[1]);
        if(len <= protocol_size)
            return 1;
        strcpy(str, http_protocol_version[1]);
        break;
    default :
        return 0;
    }
    return 0;
}

// convert reason code into string, ie 404 -> Not Found
// maximum length is 17 for supported codes, you must ajust if implemented new

int reason_code_to_str(char *str, http_reason_code_t rt)
{

    size_t len=REASON_CODE_NAME_MAX_LENGTH;
    size_t reason_code_size;

    switch(rt)
    {
    case Bad_Request:
        reason_code_size=strlen(reason_code_name[0]);
        if(len <= reason_code_size)
            return 1;
        strcpy(str, reason_code_name[0]);
        break;
    case Not_found:
        reason_code_size=strlen(reason_code_name[1]);
        if(len <= reason_code_size)
            return 2;
        strcpy(str,reason_code_name[1]);
        break;
    case Entity_Too_Large:
        reason_code_size=strlen(reason_code_name[2]);
        if(len <= reason_code_size)
            return 2;
        strcpy(str,reason_code_name[2]);
        break;
    case OK:
        reason_code_size=strlen(reason_code_name[3]);
        if(len <= reason_code_size)
            return 3;
        strcpy(str,reason_code_name[3]);
        break;
    case Internal_Error:
        reason_code_size=strlen(reason_code_name[4]);
        if(len <= reason_code_size)
            return 4;
        strcpy(str,reason_code_name[4]);
        break;
    case Not_implemented:
        reason_code_size=strlen(reason_code_name[5]);
        if(len <= reason_code_size)
            return 5;
        strcpy(str,reason_code_name[5]);
        break;
    }
    return 0;
}


http_tuple_t init_tuple(int key, const char value[128])
{
    http_tuple_t t;

    if((t = (http_tuple_t)malloc(sizeof(__http_tuple_t))) == NULL)
        return NULL;
    strncpy(t->http_header_value, value, 128);
    t->http_header=key;

    return t;
}

void destroy_tuple(http_tuple_t t)
{
    if(t == NULL)
        return;
    free(t);

    return;
}

http_conn_headers_t* init_headers()
{
    http_conn_headers_t *h;

    if((h=(http_conn_headers_t*)malloc((sizeof (http_conn_headers_t)))) == NULL)
        return NULL;

    h->entity_headers.type=http_entity_header;
    h->general_headers.type=http_general_header;
    h->request_headers.type=http_request_header;
    h->response_headers.type=http_response_header;

    h->entity_headers.headers_size=0;
    h->general_headers.headers_size=0;
    h->request_headers.headers_size=0;
    h->response_headers.headers_size=0;
    return h;

}

// function deletes all allocated tuples
void destroy_conn_headers(http_conn_headers_t* headers)
{

    for( size_t i=0; i < headers->entity_headers.headers_size; i++)
        destroy_tuple(headers->entity_headers.headers[i]);

    for( size_t i=0; i < headers->general_headers.headers_size; i++)
        destroy_tuple(headers->general_headers.headers[i]);

    for( size_t i=0; i < headers->request_headers.headers_size; i++)
        destroy_tuple(headers->request_headers.headers[i]);

    for( size_t i=0; i < headers->response_headers.headers_size; i++)
        destroy_tuple(headers->response_headers.headers[i]);

    free(headers);

    return;
}

http_general_header_t find_http_general_header(const char *gh)
{
    http_general_header_t result = Cache_Control; /* value corresponding to etable[0] */
    int i = 0;
    for (i = 0; general_header[i] != NULL; ++i, ++result)
        if (!(strncmp(gh, general_header[i], 16)))
            return result;
    return INVALID_GENERAL_HEADER;
}

http_entity_header_t find_http_entity_header(const char *eh)
{
    http_entity_header_t result = Allow; /* value corresponding to etable[0] */
    int i = 0;
    for (i = 0; entity_header[i] != NULL; ++i, ++result)
        if (!(strncmp(eh, entity_header[i], 16)))
            return result;
    return INVALID_ENTITY_HEADER;
}
http_response_header_t find_http_response_header(const char *resph)
{
    http_response_header_t result = Accept_Ranges; /* value corresponding to etable[0] */
    int i = 0;
    for (i = 0; response_header[i] != NULL; ++i, ++result)
        if (!(strncmp(resph, response_header[i], 16)))
            return result;
    return INVALID_RESPONSE_HEADER;
}

http_request_header_t find_http_request_header(const char *reqh)
{
    http_request_header_t result = Accept; /* value corresponding to etable[0] */
    int i = 0;
    for (i = 0; request_header[i] != NULL; ++i, ++result)
        if (!(strncmp(reqh, request_header[i], 16)))
            return result;
    return INVALID_REQUEST_HEADER;
}

// defines header type by its name
// input: char* header_name - name of header like Host, Content-Type etc
// output: http_header_type_t
// returns http_header_type_t
http_header_type_t detect_header_type(const char* header_name)
{

    char h_name[21];// header name cannot be more 21 bytes
    strncpy(h_name, header_name,21);

    if(find_http_general_header(h_name) != INVALID_GENERAL_HEADER)
        return http_general_header;
    if(find_http_entity_header(h_name) != INVALID_ENTITY_HEADER)
        return http_entity_header;
    if(find_http_response_header(h_name) != INVALID_RESPONSE_HEADER)
        return http_response_header;
    if(find_http_request_header(h_name) != INVALID_REQUEST_HEADER)
        return http_request_header;

    return INVALID_HEADER_TYPE;


}

int add_header_tuple(http_conn_headers_t* headers, http_header_type_t type, const http_tuple_t header_tuple)
{
    switch (type)
    {
    case http_entity_header:
        headers->entity_headers.headers[headers->entity_headers.headers_size++]=header_tuple;
        break;
    case http_general_header:
        headers->general_headers.headers[headers->general_headers.headers_size++]=header_tuple;
        break;
    case http_response_header:
        headers->response_headers.headers[headers->response_headers.headers_size++]=header_tuple;
        break;
    case http_request_header:
        headers->request_headers.headers[headers->request_headers.headers_size++]=header_tuple;
        break;
    default:
        return 1;
    }

    return 0;

}
// creates tuple with right http_header_type_t from char* name and then put into http_headers_t*
// input: const char* header_name
// output: no
// returns 0 if OK, 1 if something wrong
int add_header_name(http_conn_headers_t* conn_headers, int header_name, const char* header_value)
{
    http_header_type_t type;
    http_tuple_t header_tuple;

    if((type=detect_header_type(header_value)) == INVALID_HEADER_TYPE)
        return 1;

    if((header_tuple=init_tuple(header_name, header_value)) == NULL)
        return -1;
    if((add_header_tuple(conn_headers, type, header_tuple)) == 1)
            return 1;

    return 0;
}

// search tuple in array
// input: tuple headers array, header enum
// output: pointer to tuple in the array
// returns pointer to tuple if found and NULL otherwise
http_tuple_t find_tuple_header(http_headers_tuples_array_t* array, int header_num)
{
    for(size_t i=0; i < array->headers_size;i++)
        if(array->headers[i]->http_header == header_num)
            return array->headers[i];
    return NULL;
}
// searching value by its enum number(header) and type(because it is enum and they are crossing) in array of headers(headers)
// input: arrays of headers http_tuple_t
// output: value of header char* value
// returns 0 if found, otherwise returns 1
int find_header_from_conn_headers(char* value, http_conn_headers_t* headers, http_header_type_t type, int header)
{
    http_tuple_t t=NULL;
    switch (type)
    {
    case http_entity_header:
        t=find_tuple_header(&headers->entity_headers, header);
        if (t != NULL)
            strncpy(value, t->http_header_value, 128);
        break;
    case http_general_header:
        t=find_tuple_header(&headers->general_headers, header);
        if (t != NULL)
            strncpy(value, t->http_header_value, 128);
        break;
    case http_request_header:
        t=find_tuple_header(&headers->request_headers, header);
        if (t != NULL)
            strncpy(value, t->http_header_value, 128);
        break;
    case http_response_header:
        t=find_tuple_header(&headers->response_headers, header);
        if (t != NULL)
            strncpy(value, t->http_header_value, 128);
        break;
    default:
        return 1;

    }
    return 0;
}


//Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
int create_status_line(char* str, http_protocol_version_t p, http_reason_code_t r)
{
    char reason_str[REASON_CODE_NAME_MAX_LENGTH]={0};
    char reason_code_str[4]={0};

    // convert int code into name of reason, ie. 501 -> "501 Not Implemented\0"
    if((reason_code_to_str(reason_str, r)))
        return -1;

    http_ptorocol_code_to_str(str,p);
    strcat(str," ");// adding space separator
    strcat(str, reason_code_str); //  writing code reason
    strcat(str," "); // adding space
    strcat(str, reason_str);

    return 0;
}

/* Requests */

int parse_raw_data(http_request_t *req, raw_client_data_t *rd)
{
    if(rd == NULL)
        return -1;
    const char *delim = "\r\n";
    char *tok;

    char** saveptr=NULL;
    char request_line[REQUEST_LINE_LENGTH];

    //strtok_r is MT-Safe function
    tok = strtok_r(rd->initial_data, delim, saveptr); // search for Request-Line
    if (strlen(tok) >= REQUEST_LINE_LENGTH)
        return 1;
    strncpy(request_line, tok, 256);

    parse_request_line(&(req->req_line), request_line);

    while(tok != NULL)
    {
        tok = strtok_r(NULL, delim, saveptr);
    }



    return 0;
}

int parse_request_line(http_request_line_t* rl, const char* rc)
{


    return 0;
}



int get_current_date_string(char*date)
{

    time_t t = time(NULL);
    struct tm *tm = gmtime(&t);// probably localtime() instead of gmtime()

    strftime(date, 128, "%c", tm);

    return 0;
}

int create_date_header(char* dheader)
{
    //temporary constant date
    char date[128]={0};
    strncpy(dheader, "Date: ", 6);
    //strncpy(dheader, "Date: Fri Dec  7 15:37:41 2018", 48);
    get_current_date_string(date);
    // return 0;
    return strncat(dheader, date, 128) ? 0 : 1;
}

int create_header_name_of_server(char* server_name, size_t len)
{
    strncpy(server_name, "Server: ", 8);
    strncat(server_name, servername, len);
    return 0;
}



int create_error_response(http_response_t* res, const http_reason_code_t code, char *error_file)
{

    return 0;
}



long get_file_size(int fd)
{
    long size, current_pos;

    current_pos =lseek(fd, 0L, SEEK_CUR); //remember current pos
    size=lseek(fd, 0L, SEEK_END); // file size
    lseek(fd,current_pos, SEEK_SET); // return to initial pos

    return size;

}



// converts file extension into mime type
// max file extension length is 5
http_content_type_t get_file_MIME_type(const char* filename)
{
    char *fileExtension;
    if ((fileExtension = strrchr(filename, '.')) == NULL)
        return INVALID_MIME;

    http_content_type_t mime = text_html;

    for (int i = 0; file_ext[i] != NULL; ++i, ++mime)
        if (!(strncmp(fileExtension, file_ext[i], 5)))
            return mime;
    return INVALID_MIME;

}

int convert_file_MIME_type_in_str(char* content_type, const char* filename)
{
    http_content_type_t ct;

    if((ct = get_file_MIME_type(filename)) == INVALID_MIME)
    {
        WriteLog("Invalid MIME type, unsupported file extension");
        return -1;
    }

    if((content_type_to_str(content_type, ct)) != 0)
    {
        WriteLog("Insufficient buffer length for writing Content Type, content_type=%d", ct);
        return -1;
    }

    return 0;
}
