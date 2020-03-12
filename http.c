#include "http.h"
#include <locale.h>
#include <time.h>
#define MAX_EVENTS 10
#define servername "SmallWebServer"

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
  "Bad Request",
  "Not Found",
  "OK",
  "Entity Too Large",
  "Internal Error",
  "Not Implemented"
};
char *content_type[] = {
  "Content-Type: text/html",
  "Content-Type: image/jpg",
  "Content-Type: application/pdf",
  "Content-Type: image/png",
  "Content-Type: video/mpeg",
  "Content-Type: text/css"
};
char *file_ext[] = {".html", ".jpg", ".pdf", ".png", ".mpeg", ".css", NULL};
char* general_header[] = {
  "Cache-Control",
  "Connection",
  "Date",
  "Pragma",
  "Trailer",
  "Transfer-Encoding",
  "Upgrade",
  "Via",
  "Warning",
  NULL
};
char* response_header[] = {
  "Accept-Ranges",
  "Age",
  "ETag",
  "Location",
  "Proxy-Authenticate",
  "Retry-After",
  "Server",
  "Vary",
  "WWW-Authenticate"
};

char* entity_header[] = {

  "Allow",
  "Content-Encoding",
  "Content-Language",
  "Content-Length",
  "Content-Location",
  "Content-MD5",
  "Content-Range",
  "Content-Type",
  "Expires",
  "Last-Modified"
};

char *request_header[] = {
  "Accept",
  "Accept-Charset",
  "Accept-Encoding",
  "Accept-Language",
  "Authorization",
  "Expect",
  "From",
  "Host",
  "If-Match",
  "If-Modified-Since",
  "If-None-Match",
  "If-Range",
  "If-Unmodified-Since",
  "Max-Forwards",
  "Proxy-Authorization",
  "Range",
  "Referer",
  "TE",
  "User-Agent",
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

int content_type_to_str(char *str, size_t len, http_content_type_t ct)
{
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

int http_ptorocol_code_to_str(char *str, size_t len, http_protocol_version_t rt)
{
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

// convert reason code into value, ie 404 -> Not Found

int reason_code_to_str(char *str, size_t len, http_reason_code_t rt)
{

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

//Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
int create_status_line(char* str, size_t len, http_protocol_version_t p, http_reason_code_t r)
{
  char reason_str[64]={0};
  char reason_code_str[4]={0};

  //  convert from code to string, i.e. 501 -> "501\0"
  if((int_to_str(reason_code_str, (int)r)) == -1)
    return -1;

  // convert int code into name of reason, ie. 501 -> "Not Implemented\0"
  if((reason_code_to_str(reason_str, 64, r)))
    return -2;

  //check if str len has enough size to write
  http_ptorocol_code_to_str(str,len,p);
  if((strlen(reason_str) + strlen(reason_code_str))  + 3 > len )
    return -3;
  strcat(str," ");// adding space separator
  strcat(str, reason_code_str); //  writing code reason
  strcat(str," "); // adding space
  strcat(str, reason_str);

  return 0;
}

/* Requests */
// parse only Request-Line
int create_http_request_from_raw_data(http_request_old_t *req, raw_client_data_t *rd)
{
  if(rd == NULL)
    return -1;
  const char *delim = " \r\n";
  char *tok;
  unsigned int i = 0;
  char tmpparam[PARAMS_STRING_LENGTH];


  // char *tmp =strdup(rd->initial_data);
  // tok = strtok(tmp,delim);
  tok = strtok(rd->initial_data, delim); // search for Request-Line
  while(tok != NULL && i < 4)
    {
      switch(i)
        {
        case 0:
          // search for request method
          if((req->method=find_http_method(tok)) == -1)
            {
              // free(tmp);
              return -1;
            }
          break;
        case 1:
          // search for params
          if(tok[0] == '/')
            {
              strncpy(tmpparam,tok+1,PARAMS_STRING_LENGTH);
              strncpy(req->params, tmpparam, PARAMS_STRING_LENGTH);
            }
          break;
        case 2:
          // search for http protocol
          if((req->http_proto=find_http_protocol_version(tok)) == -1)
            {
              // free(tmp);
              return -2;
            }
          break;
        case 3:
          {
            WriteLog("case 3: tok=%s", tok);
            if((strncmp(tok, "Host:", 5)) == 0)
              strncpy(req->host,tok+5,8192);
          }
          break;
        }
      i++;
      tok = strtok(NULL, delim);
    }
  if( i != 4 )
    return -3;
  // free(tmp);
  WriteLog("Host:%s",req->host);
  return 0;
}

http_request_old_t* create_request()
{
  http_request_old_t* htp;

  if((htp = (http_request_old_t*)malloc(sizeof(http_request_old_t))) == NULL) // I don't use memset() to zeroing or calloc to improve performance. Instead of this I decided to add 0 to end of data
    return NULL;

  return htp;
}

void delete_http_request(http_request_old_t* ptr)
{
  if(ptr == NULL)
    {
      WriteLog("http request cannot be deleted as it doesn't exist");
      return;
    }
  free(ptr);
}


raw_client_data_t* create_raw_data()
{
  raw_client_data_t* rd;
  if((rd = (raw_client_data_t*)malloc(sizeof(raw_client_data_t))) == NULL)
    return NULL;

  return rd;
}

void delete_raw_data(raw_client_data_t* rd)
{
  if(rd == NULL)
    {
      WriteLog("Raw client data cannot be deleted as they don't exist");
      return;
    }
  free(rd);
}

int get_current_date_string(char* date, size_t n)
{

  time_t t = time(NULL);
  struct tm *tm = gmtime(&t);

  strftime(date, n, "%c", tm);

  return 0;
}

int create_date_header(char* dheader, size_t n)
{
  //temporary constant date
  char date[128]={0};
  strncpy(dheader, "Date: ", 6);
  //strncpy(dheader, "Date: Fri Dec  7 15:37:41 2018", 48);
  get_current_date_string(date, 128);
  // return 0;
  return strncat(dheader, date, n) ? 0 : 1;
}

int create_header_name_of_server(char* server_name, size_t len)
{
  strncpy(server_name, "Server: ", 8);
  strncat(server_name, servername, len);
  return 0;
}

http_response_old_t* create_http_response(void)
{
  http_response_old_t* resp;
  if((resp = (http_response_old_t*)malloc(sizeof(http_response_old_t))) == NULL)
    return NULL;

  return resp;
}

void fill_http_response(http_response_old_t *resp, const char* status_line)
{

  char date[128]={0}, server[128]={0};

  create_date_header(date, 128);
  create_header_name_of_server(server, 128);
  strcpy(resp->header.status_line, status_line);
  strcpy(resp->header.content_length,"Content-Length=0");// filling this header placed in further in convert_content_length()
  strcpy(resp->header.content_type, "Content-Type: text/html"); // default MIME type
  strncpy(resp->header.server, server, strlen(server)+1);
  strncpy(resp->header.date, date, strlen(date));
  resp->message_body=0;//must be file descriptor, initially equal zero

}
void delete_http_response(http_response_old_t *hp)
{
  if(hp == NULL)
    {
      WriteLog("http response cannot be deleted as it doesn't exist");
      return;
    }
  free(hp);
}

int create_200_response(http_response_old_t* res, const http_request_old_t* req)
{
  char status_line[128]={0};
  int ret;
  if((ret = create_status_line(status_line, 128, req->http_proto, OK)) == -1)
    return -1;
  fill_http_response(res, status_line);

  return 0;


}
int create_404_response(http_response_old_t *res, const http_request_old_t* req)
{
  char status_line[128]={0};
  int ret;
  if((ret = create_status_line(status_line, 128, req->http_proto, Not_found)) == -1)
    return -1;
  fill_http_response(res, status_line);

  return 0;
}
int create_413_response(http_response_old_t *res, const http_request_old_t* req)
{
  char status_line[128]={0};
  int ret;
  if((ret = create_status_line(status_line, 128, req->http_proto, Entity_Too_Large)) == -1)
    return -1;
  fill_http_response(res, status_line);

  return 0;
}
int create_500_response(http_response_old_t *res, const http_request_old_t* req)
{
  char status_line[128]={0};
  int ret;
  if((ret = create_status_line(status_line, 128, req->http_proto, Internal_Error)) == -1)
    return -1;
  fill_http_response(res, status_line);

  return 0;
}

int create_501_response(http_response_old_t * res, const http_request_old_t* req)
{
  char status_line[128]={0};
  int ret;
  ret = create_status_line(status_line, 128, req->http_proto, Not_implemented);
  if(ret < 0)
    return -1;
  fill_http_response(res, status_line);

  return 0;
}

int create_400_response(http_response_old_t *res, const http_request_old_t* req)
{
  char status_line[128]={0};
  int ret;
  if((ret = create_status_line(status_line, 128, req->http_proto, Bad_Request)) == -1)
    return -1;
  fill_http_response(res, status_line);

  return 0;
}

size_t create_serialized_http_header(char* headers, const http_response_header_old_t *rs, size_t len)
{
  size_t ret;
  ret=(size_t)snprintf(headers, len,"%s\n%s\n%s\n%s\n%s\r\n\r\n",rs->status_line, rs->server, rs->date, rs->content_type, rs->content_length);

  return ret;
}
long get_file_size(int fd)
{
  long size, current_pos;

  current_pos =lseek(fd, 0L, SEEK_CUR); //remember current pos
  size=lseek(fd, 0L, SEEK_END); // file size
  lseek(fd,current_pos, SEEK_SET); // return to initial pos

  return size;

}

int convert_Content_Length(http_response_header_old_t* header)
{
  char num[22]={0};//for 64 bin 2^64(long) has 21 symbols
  strncpy(header->content_length,"Content-Length: ",16);
  long_to_str(num,header->content_length_num);
  strncat(header->content_length, num, 22);
  return 0;
}
// converts file extension into mime type
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

int get_file_MIME_type_in_str(char* content_type, size_t len, const char* filename)
{
  http_content_type_t ct;

  if((ct = get_file_MIME_type(filename)) == INVALID_MIME)
    {
      WriteLog("Invalid MIME type, unsupported file extension");
      return -1;
    }

  if((content_type_to_str(content_type, len, ct)) != 0)
    {
      WriteLog("Insufficient buffer length for writing Content Type, content_type=%d", ct);
      return -1;
    }

  return 0;
}
