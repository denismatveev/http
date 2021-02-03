#include "http.h"
#include <locale.h>
#include <time.h>
// Based on RFC 2616
// http://tools.ietf.org/html/rfc2616
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
            return 1;
        strcpy(str,content_type[1]);
        break;
    case application_pdf:
        content_type_length=strlen(content_type[2]);
        if(len <= content_type_length)
            return 1;
        strcpy(str,content_type[2]);
        break;
    case image_png:
        content_type_length=strlen(content_type[3]);
        if(len <= content_type_length)
            return 1;
        strcpy(str,content_type[3]);
        break;
    case video_mpeg:
        content_type_length=strlen(content_type[4]);
        if(len <= content_type_length)
            return 1;
        strcpy(str,content_type[4]);
        break;
    case text_css:
        content_type_length=strlen(content_type[5]);
        if(len <= content_type_length)
            return 1;
        strcpy(str,content_type[5]);
        break;
    default:
        return 1;
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
        return 1;
    }
    return 0;
}

// convert reason code into string, ie 404 -> Not Found
// maximum length is 17 for supported codes, you must ajust if implemented new

int reason_code_to_str(char *str, http_reason_code_t rt)
{

    switch(rt)
    {
    case Bad_Request:
        if(REASON_CODE_NAME_MAX_LENGTH < strlen(reason_code_name[0]))
            return 1;
        strcpy(str, reason_code_name[0]);
        break;
    case Not_found:
        if(REASON_CODE_NAME_MAX_LENGTH < strlen(reason_code_name[1]))
            return 1;
        strcpy(str,reason_code_name[1]);
        break;
    case Entity_Too_Large:
        if(REASON_CODE_NAME_MAX_LENGTH < strlen(reason_code_name[2]))
            return 1;
        strcpy(str,reason_code_name[2]);
        break;
    case OK:
        if(REASON_CODE_NAME_MAX_LENGTH < strlen(reason_code_name[3]))
            return 1;
        strcpy(str,reason_code_name[3]);
        break;
    case Internal_Error:
        if(REASON_CODE_NAME_MAX_LENGTH < strlen(reason_code_name[4]))
            return 1;
        strcpy(str,reason_code_name[4]);
        break;
    case Not_implemented:
        if(REASON_CODE_NAME_MAX_LENGTH < strlen(reason_code_name[5]))
            return 1;
        strcpy(str,reason_code_name[5]);
        break;
    }
    return 0;
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

// function must pass args char* str buffer not more that HTTP_HEADER_NAME_MAX_LEN
int http_general_header_to_str(http_general_header_t h, char* str)
{
    switch (h)
    {
    case Cache_Control:
        if(strlen(general_header[0]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, general_header[0]);
        break;
    case Connection:
        if(strlen(general_header[1]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, general_header[1]);
        break;
    case Date:
        if(strlen(general_header[2]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, general_header[2]);
        break;
    case Pragma:
        if(strlen(general_header[3]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, general_header[3]);
        break;
    case Trailer:
        if(strlen(general_header[4]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, general_header[4]);
        break;
    case Transfer_Encoding:
        if(strlen(general_header[5]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, general_header[5]);
        break;
    case Upgrade:
        if(strlen(general_header[6]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, general_header[6]);
        break;
    case Via:
        if(strlen(general_header[7]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, general_header[7]);
        break;
    case Warning:
        if(strlen(general_header[8]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, general_header[8]);
        break;
    default:
        return 1;
    }
    return 0;
}

int http_request_header_to_str(http_request_header_t h, char* str)
{
    switch (h)
    {
    case Accept:
        if(strlen(request_header[0]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[0]);
        break;
    case Accept_Charset:
        if(strlen(request_header[1]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[1]);
        break;
    case Accept_Encoding:
        if(strlen(request_header[2]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[2]);
        break;
    case Accept_Language:
        if(strlen(request_header[3]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[3]);
        break;
    case Authorization:
        if(strlen(request_header[4]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[4]);
        break;
    case Expect:
        if(strlen(request_header[5]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[5]);
        break;
    case From:
        if(strlen(request_header[6]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[6]);
        break;
    case Host:
        if(strlen(request_header[7]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[7]);
        break;
    case If_Match:
        if(strlen(request_header[8]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[8]);
        break;
    case If_Modified_Since:
        if(strlen(request_header[9]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[9]);
        break;
    case If_None_Match:
        if(strlen(request_header[10]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[10]);
        break;
    case If_Range:
        if(strlen(request_header[11]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[11]);
        break;
    case If_Unmodified_Since:
        if(strlen(request_header[12]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[12]);
        break;
    case Max_Forwards:
        if(strlen(request_header[13]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[13]);
        break;
    case Proxy_Authorization:
        if(strlen(request_header[14]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[14]);
        break;
    case Range:
        if(strlen(request_header[15]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[15]);
        break;
    case Referer:
        if(strlen(request_header[16]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[16]);
        break;
    case TE:
        if(strlen(request_header[17]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[17]);
        break;
    case User_Agent:
        if(strlen(request_header[18]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, request_header[18]);
        break;
    default:
        return 1;

    }

    return 0;
}

int http_entity_header_to_str(http_entity_header_t h, char* str)
{
    switch (h)
    {
    case     Allow:
        if(strlen(entity_header[0]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, entity_header[0]);
        break;
    case Content_Encoding:
        if(strlen(entity_header[1]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, entity_header[1]);
        break;
    case Content_Language:
        if(strlen(entity_header[2]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, entity_header[2]);
        break;
    case Content_Length:
        if(strlen(entity_header[3]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, entity_header[3]);
        break;
    case Content_Location:
        if(strlen(entity_header[4]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, entity_header[4]);
        break;
    case Content_MD5:
        if(strlen(entity_header[5]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, entity_header[5]);
        break;
    case Content_Range:
        if(strlen(entity_header[6]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, entity_header[6]);
        break;
    case Content_Type:
        if(strlen(entity_header[7]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, entity_header[7]);
        break;
    case Expires:
        if(strlen(entity_header[8]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, entity_header[8]);
        break;
    case Last_Modified:
        if(strlen(entity_header[9]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, entity_header[9]);
        break;

    default:
        return 1;

    }

    return 0;

}
int http_response_header_to_str(http_response_header_t h, char* str)
{
    switch(h)
    {
    case Accept_Ranges:
        if(strlen(response_header[0]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, response_header[0]);
        break;
    case  Age:
        if(strlen(response_header[1]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, response_header[1]);
        break;
    case   ETag:
        if(strlen(response_header[2]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, response_header[2]);
        break;
    case Location:
        if(strlen(response_header[3]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, response_header[3]);
        break;
    case Proxy_Authenticate:
        if(strlen(response_header[4]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, response_header[4]);
        break;
    case Retry_After:
        if(strlen(response_header[5]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, response_header[5]);
        break;
    case Server:
        if(strlen(response_header[6]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, response_header[6]);
        break;
    case Vary:
        if(strlen(response_header[7]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, response_header[7]);
        break;
    case  WWW_Authenticate:
        if(strlen(response_header[8]) > HTTP_HEADER_NAME_MAX_LEN)
            return 1;
        strcpy(str, response_header[8]);
        break;

    default:
        return 1;
    }
    return 0;
}
http_header_node_t* init_http_request_header_node(const char http_header_name[HTTP_HEADER_NAME_MAX_LEN], const char http_header_value[HTTP_HEADER_VALUE_MAX_LEN])
{
    http_header_node_t* t;
    char http_header[HTTP_HEADER_NAME_MAX_LEN];

    strncpy(http_header,http_header_name, HTTP_HEADER_NAME_MAX_LEN);

    if((t = (http_header_node_t*)malloc(sizeof(http_header_node_t))) == NULL)
        return NULL;
    strncpy(t->http_header_value, http_header_value, HTTP_HEADER_VALUE_MAX_LEN);
    t->next = NULL;

    if((find_http_general_header(http_header)) != INVALID_GENERAL_HEADER)
        t->type=http_general_header;
    else if(find_http_entity_header(http_header) != INVALID_ENTITY_HEADER)
        t->type=http_entity_header;
    else if(find_http_request_header(http_header) != INVALID_REQUEST_HEADER)
        t->type=http_request_header;
    else return NULL;

    return t;
}
http_header_node_t* init_http_response_header_node(const char http_header_name[HTTP_HEADER_NAME_MAX_LEN], const char http_header_value[HTTP_HEADER_VALUE_MAX_LEN])
{
    http_header_node_t* t;
    char http_header[HTTP_HEADER_NAME_MAX_LEN];

    strncpy(http_header,http_header_name, HTTP_HEADER_NAME_MAX_LEN);

    if((t = (http_header_node_t*)malloc(sizeof(http_header_node_t))) == NULL)
        return NULL;
    strncpy(t->http_header_value, http_header_value, HTTP_HEADER_VALUE_MAX_LEN);
    t->next = NULL;

    if((find_http_general_header(http_header)) != INVALID_GENERAL_HEADER)
        t->type=http_general_header;
    else if(find_http_entity_header(http_header) != INVALID_ENTITY_HEADER)
        t->type=http_entity_header;
    else if(find_http_response_header(http_header) != INVALID_REQUEST_HEADER)
        t->type=http_request_header;
    else return NULL;

    return t;
}
void destroy_http_headers_node(http_header_node_t* header_node)
{
    free(header_node);

    return;
}

http_headers_list_t* init_http_headers_list(const http_header_node_t* first_node)
{
    http_headers_list_t* hl;
    if((hl=(http_headers_list_t*)malloc(sizeof(http_header_node_t))) == NULL)
        return NULL;
    hl->first=first_node;
    hl->limit=64;
    hl->capacity=0;

    return hl;
}
void destroy_http_headers_list(http_headers_list_t* hl)
{
    http_header_node_t* current=hl->first;
    http_header_node_t* tmp;

    while(current->next != NULL)
    {
        tmp=current->next;
        destroy_http_headers_node(current);
        current=tmp;
    }
    destroy_http_headers_node(current);

    free(hl);
    return;
}

int add_http_header_to_list(http_headers_list_t* list, const http_header_node_t* header)
{
    http_header_node_t* current;

    current=list->first;

    while(current->next != NULL)
        current=current->next;

    current->next=header;

    return 0;
}

int create_http_request(http_request_t* request, const http_headers_list_t* list, http_request_line_t req_line)
{
    request->headers=list;
    request->req_line.method=req_line.method;
    request->req_line.http_version=req_line.http_version;
    strncpy(request->req_line.request_URI, req_line.request_URI, REQUEST_URI_STRING_LENGTH);

    return 0;
}

int create_http_response(http_response_t *response, http_headers_list_t* list, http_status_line_t status)
{

    response->sl=status;
    response->headers=list;

    return 0;
}
int find_header_name_value_by_type(const http_header_node_t* node, char name[HTTP_HEADER_NAME_MAX_LEN], char value[HTTP_HEADER_VALUE_MAX_LEN] )
{

    switch (node->type)
    {
    case http_entity_header:
        http_entity_header_to_str(node->http_header, name);
        strncpy(value, node->http_header_value, HTTP_HEADER_VALUE_MAX_LEN);
        break;
    case http_general_header:
        http_general_header_to_str(node->http_header, name);
        strncpy(value, node->http_header_value, HTTP_HEADER_VALUE_MAX_LEN);
        break;
    case http_request_header:
        http_request_header_to_str(node->http_header, name);
        strncpy(value, node->http_header_value, HTTP_HEADER_VALUE_MAX_LEN);
        break;
    case http_response_header:
        http_response_header_to_str(node->http_header,name);
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
    strcat(str, SP);// adding space separator
    strcat(str, reason_code_str); //  writing code reason
    strcat(str, SP); // adding space
    strcat(str, reason_str);

    return 0;
}

/* Requests */

int parse_raw_data(http_request_t *req, raw_client_data_t *rd)
{
    if(rd == NULL)
        return -1;
    char *tok;
    char header[HTTP_HEADER_NAME_MAX_LEN], value[HTTP_HEADER_VALUE_MAX_LEN];

    char** saveptr=NULL;
    char request_line[REQUEST_LINE_LENGTH];

    //strtok_r is MT-Safe function
    if((tok = strtok_r(rd->initial_data, CRLF, saveptr)) == NULL)// search for Request-Line
        return 1;

    if (strlen(tok) >= REQUEST_LINE_LENGTH)
        return 1;
    strncpy(request_line, tok, REQUEST_LINE_LENGTH);

    if((parse_request_line(&(req->req_line), request_line)) != 0)
        return 1;

    while(tok != NULL)
    {
        tok = strtok_r(NULL, CRLF, saveptr);
        if((parse_http_header_line(tok, header, value)) != 0)
            return 1;
    }

    return 0;
}
int parse_http_header_line(char* header_line, char header[HTTP_HEADER_NAME_MAX_LEN], char value[HTTP_HEADER_VALUE_MAX_LEN])
{
    char * tok;
    char** saveptr=NULL;




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
