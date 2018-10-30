#include"http.h"
#define MAX_EVENTS 10
/* array of strings to search symbol name of HTTP method */
char *http_method[] = {"GET", "POST", "HEAD", "PUT", "DELETE", "CONNECT", "OPTIONS","TRACE", "PATCH", NULL};
/* only two versions supported */
char *http_protocol_version[] = {"HTTP/1.1", "HTTP/2", NULL};
char *reason_code[] = {"Bad Request", "Not Found", "OK"};


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

int http_request_from_raw_data(http_request_t *req, const raw_client_data_t *rd)
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
          if((req->method=find_http_method(tok)) == -1)
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
          if((req->http_proto=find_http_protocol_version(tok)) == -1)
            {
              free(tmp);
              return -1;
            }
          break;
        }
      i++;
      tok = strtok(NULL, delim);
    }
  if( i != 3 )
    return -1;
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

void delete_request(http_request_t* ptr)
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


// convert reason code into value, ie 404 -> Not Found
int reason_code_to_str(char *str, size_t len, http_reason_code_t rt)
{

  size_t reason_code_size;

  switch(rt)
    {
    case 400:
      reason_code_size=strlen(reason_code[0]);
      if(len <= reason_code_size)
        return 1;
      strcpy(str, reason_code[0]);
      break;
    case 404:
      reason_code_size=strlen(reason_code[1]);
      if(len <= reason_code_size)
        return 1;
      strcpy(str,reason_code[1]);
      break;
    case 200:
      reason_code_size=strlen(reason_code[2]);
      if(len <= reason_code_size)
        return 1;
      strcpy(str,reason_code[2]);
      break;
deafult:
      return -1;
    }
  str[reason_code_size] = 0;

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

// response consists of two strings:
// response header
// message body
// temporary
http_response_t* create_response(const char* status_line)
{
  http_response_t* resp;
  if((resp = (http_response_t*)malloc(sizeof(http_response_t))) == NULL)
    return NULL;


  strcpy(resp->header.status_line, status_line);


  resp->header.content_length=0;
  strcpy(resp->header.content_type, "Content-Type: text/html");
  strcpy(resp->header.server,"Server: nginx/1.4.4");
  strcpy(resp->header.date,"Date: Tue, 30 Oct 2018 19:18:12 GMT");
  resp->message_body=4;

  return resp;
}
int delete_http_response(http_response_t *hp)
{
  free(hp);
}

// parse request and create a response
int run_request(http_response_t* resp, const http_request_t *ht, int client_sock)
{
  int fd;
  //  fd=open();


  //  sendfile(client_sock, fd, NULL, );
}
