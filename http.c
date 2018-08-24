#include"http.h"

#define MAX_EVENTS 10
/* array of strings to search symbol name of HTTP method */
char *http_method[] = {"GET", "POST", "HEAD", "PUT", "DELETE", "CONNECT", "OPTIONS","TRACE", "PATCH", NULL};
/* only two versions supported */
char *http_protocol_version[] = {"HTTP/1.1", "HTTP/2", NULL};
char *reason_code[] = {"Bad Request", "Not Found", "OK"};
char *response_header[] = {  "Age",
                             "Location",
                             "Proxy-Authenticate",
                             "Public",
                             "Retry-After",
                             "Server",
                             "Vary",
                             "Warning",
                             "WWW-Authenticate"
                          };


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

http_request_t* fill_http_request(const char *string_req)
{
  if(string_req == NULL)
    return NULL;
  const char *delim = " ";
  char *tok;
  unsigned int i = 0;
  char *tmp;
  if((tmp = strdup(string_req)) == NULL)
    return NULL;
  tok = strtok(tmp,delim);

  http_request_t* ht;

  if((ht=(http_request_t*)malloc(sizeof(http_request_t))) == NULL)
    return NULL;

  while(tok != NULL && i < 3)
  {
    switch(i)
    {
    case 0:
      // search for request method
      if((ht->method=find_http_method(tok)) == -1)
        goto free;
      break;
    case 1:
      // search for params
      strncpy(ht->params, tok, PARAMS_STRING_LENGTH);
      break;
    case 2:
      // search for http protocol
      if((ht->http_proto=find_http_protocol_version(tok)) == -1)
        goto free;
      break;
    }
    i++;
    tok = strtok(NULL, delim);
  }
  if( i != 3 )
    goto free;

  free(tmp);
  return ht;
free:
    free(tmp);
    free(ht);
    return NULL;
}

destroy_http_request(http_request_t * ht)
{
  free(ht);
}
