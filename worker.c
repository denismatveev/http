#include "worker.h"
#include "common.h"
#include "http.h"
#include "parse_config.h"
#include <pthread.h>
#include <stdlib.h>
#include "jobs_queue.h"
#include <errno.h>
#include <sys/sendfile.h>

// shared variables between threads
pthread_cond_t output_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t input_cond = PTHREAD_COND_INITIALIZER;
jobs_queue_t* input_queue;
jobs_queue_t* output_queue;



int create_listener()
{
  extern config_t cfg;

  int sockfd;
  struct sockaddr_in serv_addr;

  serv_addr.sin_port=cfg.listen.sin_port;
  serv_addr.sin_family=cfg.listen.sin_family;
  serv_addr.sin_addr=cfg.listen.sin_addr;
  const int on = 1;

  memset(&serv_addr.sin_zero,0,8);


  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
      WriteLogPError("Error opening socket");
      exit(EXIT_FAILURE);
    }
  if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) // use it before bind(). It provides immediately server restart before TIME_WAIT is expired
    WriteLogPError("Error calling setsockopt()");
  if((bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0))
    {
      WriteLogPError("Cannot bind socket");
      exit(EXIT_FAILURE);
    }

  // setnonblocking(sockfd);
  if((listen(sockfd, 255)) < 0)
    {
      WriteLogPError("Can't listen to the socket");
      exit(EXIT_FAILURE);
    }

  return sockfd;
}

void create_worker()
{
  int conn_sock, nfds, epollfd, n, sockfd, err;
  struct epoll_event ev, events[MAX_EVENTS];
  epollfd = epoll_create1(0);
  ev.events = EPOLLIN;// level triggered
  struct sockaddr_in cli_addr;
  socklen_t clilen;

  //+++++++++++++++++++++++++++
  sockfd=create_listener();

  // queues initializing
  input_queue = init_jobs_queue();
  output_queue = init_jobs_queue();
  run_threads();

  ev.data.fd = sockfd;


  epollfd = epoll_create1(0);
  if (epollfd == -1)
    {
      WriteLogPError("epoll_create1");
      exit(EXIT_FAILURE);
    }


  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
    {
      WriteLogPError("epoll_ctl");
      exit(EXIT_FAILURE);
    }
  //TODO the cycle below to be placed into dedicated thread
  for (;;)
    {
      nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
      if (nfds == -1)
        {
          err=errno;
          if(err ==EINTR)
            continue;
          else
            {
              WriteLogPError("epoll_wait");
              exit(EXIT_FAILURE);
            }
        }

      for (n = 0; n < nfds; ++n)
        { // event on listening socket
          if (events[n].data.fd == sockfd)
            {
              conn_sock = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
              if (conn_sock == -1)
                {
                  WriteLogPError("accept");
                  exit(EXIT_FAILURE);
                }
              // setnonblocking(conn_sock); // must be used in edge-triggered(ET) mode

              // ev.events = EPOLLIN | EPOLLET; // ET mode
              ev.events = EPOLLIN;
              ev.data.fd = conn_sock;
              if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,&ev) == -1)
                {
                  WriteLogPError("epoll_ctl: conn_sock");
                  exit(EXIT_FAILURE);
                }
            }
          else
            {
              // event on accepted socket
              // create job and place into input queue
              create_job_with_raw_data_and_place_into_input_queue(events[n].data.fd);
              // number of threads are doing work
              // process_jobs(input_queue, output_queue);
              // one thread to send data
              // send_data_from_output_queue(output_queue);

            }
        }
    }
}

int create_job_with_raw_data_and_place_into_input_queue(int client_sock)
{
  job_t *job;
  ssize_t bytes_read;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  if((job = create_job()) == NULL)
    return -1;

  job->raw_data->client_socket=client_sock; //  copy fd to raw data structure
  bytes_read=read(job->raw_data->client_socket, job->raw_data->initial_data, INITIAL_DATA_SIZE); // reading data from the socket to structure
  job->raw_data->initial_data[bytes_read+1] = 0;//adds 0 to the end of data

  if(push_job(input_queue, job))
    {
      WriteLog("Error occured while pushing a job into jobs queue\n");
      destroy_job(job);
      return -1;
    }
  // pthread_cond_broadcast or pthread_cond_signal?

  pthread_mutex_lock(&mutex);
  pthread_cond_broadcast(&input_cond); // new job appeared in input queue, sending a signal to wake up all threads
  pthread_mutex_unlock(&mutex);

  return 0;
}

// take job from one queue(input_queue), do somethibg and put into another(output_queue)
void* process_jobs(void *args)
{
  //  struct sending_thread_args *targs = args;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  // pthread_mutex_init(&mutex, NULL);
  job_t *job;
  int err;
  int ret;
  int fd; // file descriptor of opened file to be sent

  while(1)
    {
      pthread_mutex_lock(&mutex);
      pthread_cond_wait(&input_cond,&mutex);
      pthread_mutex_unlock(&mutex);
      if(pop_job(input_queue, &job))
        {
          WriteLog("Error occured while popping up a job from jobs queue\n");
          // return -1;
          break;
        }

      // parsing raw client data and create a http_request
      ret=create_http_request_from_raw_data(job->req, job->raw_data);
      // depending on what data are in http_request we are making a reply
      // TODO implement HEAD request
      if(ret < 0 )
        {
          // bad request
          create_400_reply(job->response, job->req);
          fd=open("error_pages/400.html",O_RDONLY );
          job->response->message_body=fd;
          job->response->header.content_length_num=findout_filesize(fd);
          convert_content_length(&job->response->header);
          goto PUSH;

        }
      if(job->req->http_proto != HTTP11)
        {
          // not implemented
          create_501_reply(job->response, job->req);

          fd=open("error_pages/501.html",O_RDONLY );
          job->response->message_body=fd;
          job->response->header.content_length_num=findout_filesize(fd);
          convert_content_length(&job->response->header);
          goto PUSH;
        }

      // finding a file and creating a reply
      if(job->req->method == GET)
        {
          fd = open(job->req->params,O_RDONLY);
          if(fd > 0)
            {
              //everything is OK
              create_200_reply(job->response, job->req);
              job->response->message_body=fd;
              job->response->header.content_length_num=findout_filesize(fd);
              convert_content_length(&job->response->header);
              goto PUSH;
            }
          else if(fd < 0)
            {
              //something went wrong
              err = errno;
              if(err == EACCES || err == ENOENT)
                {
                  //no such file
                  create_404_reply(job->response, job->req);
                  fd=open("error_pages/404.html",O_RDONLY );
                  job->response->message_body=fd;
                  job->response->header.content_length_num=findout_filesize(fd);
                  convert_content_length(&job->response->header);
                  goto PUSH;

                }
              else if(err == ENFILE || err == ELOOP)
                {
                  //specific errors
                  create_500_reply(job->response, job->req);
                  fd=open("error_pages/500.html",O_RDONLY );
                  job->response->message_body=fd;
                  job->response->header.content_length_num=findout_filesize(fd);
                  convert_content_length(&job->response->header);
                  goto PUSH;
                }
            }
        }
      else
        {
          // not implemented
          create_501_reply(job->response, job->req);
          fd=open("error_pages/501.html",O_RDONLY );
          job->response->message_body=fd;
          job->response->header.content_length_num=findout_filesize(fd);
          convert_content_length(&job->response->header);
          goto PUSH;
        }

PUSH:
      // pushing the job into output queue
      if(push_job(output_queue, job))
        {
          WriteLog("Error occured while pushing a job into jobs queue\n");
          //return -1;
          break;
        }
      pthread_mutex_lock(&mutex);
      pthread_cond_signal(&output_cond); // data placed in output queue
      pthread_mutex_unlock(&mutex);
    }
}
void* send_data_from_output_queue(void* args)
{

  struct sending_thread_args *targs = args;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  job_t* job;
  char serialized_headers[512]={0};
  size_t headers_len;
  ssize_t sent_bytes=0;

  while(1)
    {
      pthread_mutex_lock(&mutex);// mutex here is needed to protect conditional variable
      pthread_cond_wait(&output_cond, &mutex);
      pthread_mutex_unlock(&mutex);

      pop_job(output_queue, &job);

      // create serialized http header
      headers_len=create_serialized_http_header(serialized_headers, &job->response->header, 512);
      // sending an answer
      // At first we need to send headers and then file
      sent_bytes = write(job->raw_data->client_socket, serialized_headers, headers_len);
      sent_bytes += sendfile(job->raw_data->client_socket, job->response->message_body, NULL, (size_t)job->response->header.content_length_num);

      // deleting all allocated resources
      close(job->response->message_body);
      // close client socket
      close(job->raw_data->client_socket);
      destroy_job(job);
    }
}

//TODO create function to close all allocated resources(queues, listening sockets) if signal came(i.e.to reload config)


int run_threads()
{
  int rc;
  pthread_t worker;
  pthread_t sender;

  pthread_attr_t worker_attr;
  pthread_attr_t sender_attr;

  if((rc=pthread_attr_init(&worker_attr)))
    return rc;

  if((rc=pthread_attr_setdetachstate(&worker_attr, PTHREAD_CREATE_DETACHED)))
    return rc;

  if((rc=pthread_attr_init(&sender_attr)))
    return rc;
  if((rc=pthread_attr_setdetachstate(&sender_attr, PTHREAD_CREATE_DETACHED)))
    return rc;

  if((rc=pthread_create(&worker, &worker_attr, process_jobs, NULL)))
    {
      WriteLogPError("processing thread pthread_create");
      pthread_attr_destroy(&worker_attr);
      return rc;
    }

  if((rc=pthread_create(&sender, &sender_attr, send_data_from_output_queue, NULL)))
    {
      WriteLogPError("sending thread pthread_create");
      pthread_attr_destroy(&sender_attr);
      return rc;
    }
  return 0;
}
