#include"http.h"
#include <locale.h>
#include <time.h>
#define MAX_EVENTS 10
#define servername "My little server"

/* General */

/* array of strings to search symbol name of HTTP method */
char *http_method[] = {"GET", "POST", "HEAD", "PUT", "DELETE", "CONNECT", "OPTIONS","TRACE", "PATCH", NULL};
/* only two versions supported */
char *http_protocol_version[] = {"HTTP/1.1", "HTTP/2", NULL};
char *reason_code_name[] = {"Bad Request", "Not Found", "OK", "Internal Error", "Not Implemented"};
char* content_type[] = {"Content-Type: html/text"};
http_method_t find_http_method(const char *sval)
{
  http_method_t result = GET; /* value corresponding to etable[0] */
  int i = 0;
  for (i = 0; http_method[i] != NULL; ++i, ++result)
    if (!(strncmp(sval, http_method[i], 16)))
      return result;
  return -1;
}
http_protocol_version_t find_http_protocol_version(const char *sval)
{
  http_protocol_version_t result = HTTP11; /* value corresponding to etable[0] */
  int i = 0;
  for (i = 0; http_protocol_version[i] != NULL; ++i, ++result)
    if (!(strncmp(sval, http_protocol_version[i], 16)))
      return result;
  return -1;
}

// convert content_type into string

int content_type_to_str(char *str, size_t len, http_content_type_t ct)
{
  size_t content_type_length;
  switch (ct)
    {
    case html_text:
      content_type_length=strlen(content_type[0]);
      if(len <= content_type_length)
        return 1;
      strcpy(str,content_type[0]);
      break;
    default:
      return -1;
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
        return 1;
      strcpy(str,reason_code_name[1]);
      break;
    case OK:
      reason_code_size=strlen(reason_code_name[2]);
      if(len <= reason_code_size)
        return 1;
      strcpy(str,reason_code_name[2]);
      break;
    case Internal_Error:
      reason_code_size=strlen(reason_code_name[3]);
      if(len <= reason_code_size)
        return 1;
      strcpy(str,reason_code_name[3]);
      break;
    case Not_implemented:
      reason_code_size=strlen(reason_code_name[4]);
      if(len <= reason_code_size)
        return 1;
      strcpy(str,reason_code_name[4]);
      break;
    default:
      return -1;
    }
  return 0;
}

//Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
int create_status_line(char* str, size_t len, http_protocol_version_t p, http_reason_code_t r)
{
  char reason_str[4];
  char reason_code_str[12];

  if((int_to_str(reason_code_str, r)) == -1)
    return -1;

  if((reason_code_to_str(reason_str, 12, r)))
    return -1;

  //check if str len has enough size to write
  if((strlen(http_protocol_version[p]) +strlen(reason_str) + strlen(reason_code_str))  + 3 > len )
    return -1;

  strcat(str, http_protocol_version[p]); // writing HTTP protocol to status line
  strcat(str," ");// adding space separator
  strcat(str, reason_code_str); //  writing code reason
  strcat(str," "); // adding space
  strcat(str, reason_str);

  return 0;
}

/* Requests */

int create_http_request_from_raw_data(http_request_t *req, const raw_client_data_t *rd)
{
  if(rd == NULL)
    return -1;
  const char *delim = " ";
  char *tok;
  unsigned int i = 0;

  char *tmp =strdup(rd->initial_data);
  tok = strtok(tmp,delim);
  while(tok != NULL && i < 3)
    {
      switch(i)
        {
        case 0:
          // search for request method
          if((req->method=find_http_method(tok)) == BAD_METHOD)
            {
              free(tmp);
              return -1;
            }
          break;
        case 1:
          // search for params
          strncpy(req->params, tok, PARAMS_STRING_LENGTH);
          break;
        case 2:
          // search for http protocol
          if((req->http_proto=find_http_protocol_version(tok)) == BAD_METHOD)
            {
              free(tmp);
              return -2;
            }
          break;
        }
      i++;
      tok = strtok(NULL, delim);
    }
  if( i != 3 )
    return -3;
  free(tmp);
  return 0;
}

http_request_t* create_request()
{
  http_request_t* htp;

  if((htp = (http_request_t*)malloc(sizeof(http_request_t))) == NULL) // I don't use memset() to zeroing or calloc to improve performance. Instead of this I decided to add 0 to end of data
    return NULL;

  return htp;
}

void delete_http_request(http_request_t* ptr)
{
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
  free(rd);
}

int get_current_date_string(char* date, size_t n)
{

  setlocale(LC_TIME,"en_EN.UTF-8");
  time_t t = time(NULL);
  struct tm *tm = gmtime(&t);

  strftime(date, n, "%c", tm);

  return 0;
}

int create_date_header(char* dheader, size_t n)
{
  char date[128];
  strncpy(dheader, "Date: ", 6);

  get_current_date_string(date, 128);

  return strncpy(dheader, date, n) ? 0 : 1;

}

int create_header_name_of_server(char* server_name, size_t len)
{
  strncpy(server_name, "Server: ", 8);
  strncpy(server_name, servername, len);

  return 0;
}

http_response_t* create_http_response(void)
{
  http_response_t* resp;
  if((resp = (http_response_t*)malloc(sizeof(http_response_t))) == NULL)
    return NULL;

  return resp;
}

void fill_http_response(http_response_t *resp, const char* status_line)
{

  char date[128], server[128];

  create_date_header(date, 128);
  create_header_name_of_server(server, 128);
  strcpy(resp->header.status_line, status_line);


  strcpy(resp->header.content_length,"Content-Length=0");// filling this header placed in futher in convert_content_length()
  strcpy(resp->header.content_type, "Content-Type: text/html"); //TODO make depending on content
  strncpy(resp->header.server,server, strlen(server));
  strncpy(resp->header.date, date, strlen(date));
  resp->message_body=0;//must be file descriptor, initially equal zero

}
void delete_http_response(http_response_t *hp)
{
  free(hp);
}

int create_200_reply(http_response_t* res, const http_request_t* req)
{
  char status_line[128];
  int ret;
  if((ret = create_status_line(status_line, 128, req->http_proto, OK)) == -1)
    return -1;
  fill_http_response(res, status_line);

  return 0;


}
int create_404_reply(http_response_t *res, const http_request_t* req)
{
  char status_line[128];
  int ret;
  if((ret = create_status_line(status_line, 128, req->http_proto, Not_found)) == -1)
    return -1;
  fill_http_response(res, status_line);

  return 0;
}

int create_500_reply(http_response_t *res, const http_request_t* req)
{
  char status_line[128];
  int ret;
  if((ret = create_status_line(status_line, 128, req->http_proto, Internal_Error)) == -1)
    return -1;
  fill_http_response(res, status_line);

  return 0;
}

int create_501_reply(http_response_t * res, const http_request_t* req)
{
  char status_line[128];
  int ret;
  if((ret = create_status_line(status_line, 128, req->http_proto, Not_implemented)) == -1)
    return -1;
  fill_http_response(res, status_line);

  return 0;
}

int create_400_reply(http_response_t *res, const http_request_t* req)
{
  char status_line[128];
  int ret;
  if((ret = create_status_line(status_line, 128, req->http_proto, Bad_Request)) == -1)
    return -1;
  fill_http_response(res, status_line);

  return 0;
}

size_t create_serialized_http_header(char* headers, http_response_header_t* rs, size_t len)
{
  size_t ret;
  ret=(size_t)snprintf(headers, len,"%s\n%s\n%s\n%s\n%s\n",rs->status_line, rs->server, rs->date, rs->content_type, rs->content_length);

  return ret;
}
long findout_filesize(int fd)
{
  long size, current_pos;

  current_pos =lseek(fd, 0L, SEEK_CUR); //remember current pos
  size=lseek(fd, 0L, SEEK_END); // file size
  lseek(fd,current_pos, SEEK_SET); // return to initial pos

  return size;

}

int convert_content_length(http_response_header_t* header)
{
  char num[22];//for 64 bin 2^64(long) has 21 symbols
  strncpy(header->content_length,"Content-Length: ",16);
  long_to_str(num,header->content_length_num);
  strncat(header->content_length, num, 22);
  return 0;
}
