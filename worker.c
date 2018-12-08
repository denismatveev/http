#include "worker.h"
#include "common.h"
#include "http.h"
#include "parse_config.h"
#include "pthread.h"
#include "stdlib.h"
#include "jobs_queue.h"
#include <errno.h>
#include <sys/sendfile.h>

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
  pthread_cond_t condition;

  jobs_queue_t* input_queue;
  jobs_queue_t* output_queue;

  input_queue = init_jobs_queue();
  output_queue = init_jobs_queue();

  sockfd=create_listener();

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
              create_job_with_raw_data_and_place_into_input_queue(input_queue, events[n].data.fd);
              process_jobs(input_queue, output_queue);
              send_data_from_output_queue(output_queue);
              shutdown(events[n].data.fd, SHUT_RD); //shutdown is needed only if got a request for disconnecting

            }
        }
    }
}

int create_job_with_raw_data_and_place_into_input_queue(jobs_queue_t* input_jobs_queue, int client_sock)
{
  job_t *job;
  ssize_t bytes_read;

  if((job = create_job()) == NULL)
    return -1;

  job->raw_data->client_socket=client_sock; //  copy fd to raw data structure
  bytes_read=read(job->raw_data->client_socket, job->raw_data->initial_data, INITIAL_DATA_SIZE); // reading data from the socket to structure
  job->raw_data->initial_data[bytes_read+1] = 0;//adds 0 to the end of data

  if(push_job(input_jobs_queue, job))
    {
      WriteLog("Error occured while pushing a job into jobs queue\n");
      destroy_job(job);
      return -1;
    }
  return 0;
}
// int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
// take job from one queue(input_queue), do somethibg and put into another(output_queue)
int process_jobs(jobs_queue_t* input, jobs_queue_t* output)
{
  job_t *job;
  int err;
  int ret;
  int fd; // file desctiptor of opened file to be sent

  if(pop_job(input, &job))
    {
      WriteLog("Error occured while popping up a job from jobs queue\n");
      return -1;

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
          WriteLogPError(job->req->params);
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
  //  pushing the job into output queue
  if(push_job(output, job))
    {
      WriteLog("Error occured while pushing a job into jobs queue\n");
      return -1;
    }
  return 0;
}


ssize_t send_data_from_output_queue(jobs_queue_t* output_queue)
{

  job_t* job;
  char serialized_headers[512]={0};
  size_t headers_len;
  ssize_t sent_bytes=0;

  pop_job(output_queue, &job);
  // create serialized http header
  headers_len=create_serialized_http_header(serialized_headers, &job->response->header, 512);
  // sending an answer
  // At first we need to send headers and then file
  sent_bytes = write(job->raw_data->client_socket, serialized_headers, headers_len);
  sent_bytes += sendfile(job->raw_data->client_socket,job->response->message_body, NULL, (size_t)job->response->header.content_length_num);

  // deleting all allocated resources
  close(job->response->message_body);
  // close client socket
  close(job->raw_data->client_socket);
  destroy_job(job);


  return sent_bytes;
}


int run_workers(int number_threads)
{
  // func creates thread and distribute jobs among them
  int i;
  // thread look into queue, wake up by signal(conditional variable), takes job, does job and sends reply into client's socket

  // TODO count CPU usage by thread and give job to least loaded thread
  // pthread_getcpuclockid and clock_gettime to calculate CPU utilization by thread
  // https://stackoverflow.com/questions/18638590/can-i-measure-the-cpu-usage-in-linux-by-thread-into-application
  //  double cpuNow( void ) {
  //      struct timespec ts;
  //      clockid_t cid;

  //      pthread_getcpuclockid(pthread_self(), &cid);
  //      clock_gettime(cid, &ts);
  //      return ts.tv_sec + (((double)ts.tv_nsec)*0.000000001);
  //  }

  for(i = 0;i < number_threads; i++)
    {
      //pthread_create();

    }
  return 0;
}
//TODO create function to close all allocated resources(queues, listening sockets) if signal come(i.e.to reload config)
