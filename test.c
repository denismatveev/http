#include"http.h"
 /* tests */
 void test_func()
 {
     int i;
     char *test_str[] = {"   GET / HTTP/1.1", "GET /index.html HTTP/1.1", "GET /index.php?a=12&bb=32 HTTP/1.1","   POST  / HTTP/1.1"};
     http_request_t q;
     printf("print parsed request:\n");
     for(i = 0; i < 4; i++)
       {
         printf("input request: %s\n", test_str[i]);
         fill_http_request(&q, test_str[i]);
         printf("method: %s\nprotocol: %s\nparams: %s\n",http_method[q.method] , http_protocol_version[q.http_proto] , q.params);
         //printf("method: %u\nprotocol: %u\nparams: %s\n",q.method , q.http_proto , q.params);
 
       }
 }

int main(int argc, char ** argv)
{
  test_func();

  return 0;
}

