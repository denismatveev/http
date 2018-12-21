#include"http.h"
#include"jobs_queue.h"
#include "parse_config.h"
/* tests */
void test_req_parsing()
{
  int i;
  char *test_str[] = {"   GET / HTTP/1.1 ", "GET /index.html HTTP/1.1", "GET /index.php?a=12&bb=32 HTTP/2 ","   POST  / HTTP/1.1"};
  http_request_t q;
  printf("print parsed request:\n");
  for(i = 0; i < 4; i++)
    {
      printf("\n********************\ninput request: %s\n", test_str[i]);
      if(fill_http_request(&q, test_str[i]))
        printf("Bad request: %s\n", test_str[i]);
      printf("method: %s\nprotocol: %s\nparams: %s\n",http_method[q.method] , http_protocol_version[q.http_proto] , q.params);
      //printf("method: %u\nprotocol: %u\nparams: %s\n",q.method , q.http_proto , q.params);

    }
}
int test_queue()
{
  char *test_str[] = {"   GET /1 HTTP/1.1 ", "GET /2 HTTP/1.1", "GET /3 HTTP/2 ","   POST  /4 HTTP/1.1"};
  http_request_t q[4];
  jobs_queue_t *queue = init_jobs_queue();
  int i,k,size;
  job_t* j;


  printf("print parsed request:\n");
  for(i = 0; i < 4; i++)
    {
      printf("\n********************\ninput request: %s\n", test_str[i]);
      if(fill_http_request(&q[i], test_str[i]))
        {
          printf("Bad request: %s\n", test_str[i]);
          break;
        }
      printf("\n***********************\nmethod: %s\nprotocol: %s\nparams: %s\n",http_method[q[i].method] , http_protocol_version[q[i].http_proto] , q[i].params);
      //printf("method: %u\nprotocol: %u\nparams: %s\n",q.method , q.http_proto , q.params);


      // printf("\n********************\ninput request: %s\n", test_str[i]);

      for(k = 0; k < 8;k++)
        {
          j=create_job(q+i);

          int o = push_job(queue, j);
          switch(o)
            {
            case -1:
              return -1;
            case 1:
              return 1;
            case 2:
              return 2;
            }
        }
    }


  size =  queue->high_bound - queue->low_bound;

  for(i = 0;i < size; i++)
    {
      pop_job(queue, &j);
      printf("\n********************\nOutput:\nmethod: %s\nproto: %s\nparams: %s\n", http_method[j->req->method] , http_protocol_version[j->req->http_proto] , j->req->params );

      destroy_job(j);

    }

  return 0;

}

int test_parse_config()
{

  config_t cfg;


  parse_cfg(&cfg, "/home/dmat/projects/http/lhttpd.cfg");


}

int main(int argc, char ** argv)
{
  test_parse_config();
//  return test_queue();
}

