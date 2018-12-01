#include "worker.h"
#include "common.h"
#include "http.h"
#include "pthread.h"
#include "stdlib.h"
#include "jobs_queue.h"
#include <errno.h>

extern config_t cfg;
jobs_queue_t* input_queue;
jobs_queue_t* output_queue;

void create_ioworker(int sockfd)
{
  int conn_sock, nfds, epollfd, n;
  struct epoll_event ev, events[MAX_EVENTS];
  epollfd = epoll_create1(0);
  ev.events = EPOLLIN;
  ev.data.fd = sockfd;

  struct sockaddr_in cli_addr;
  socklen_t clilen;
  pthread_cond_t condition;


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
  //TODO the cycle below place into dedicated thread
  for (;;)
    {
      nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
      if (nfds == -1)
        {
          WriteLogPError("epoll_wait");
          exit(EXIT_FAILURE);
        }

      for (n = 0; n < nfds; ++n)
        { //event on listening socket
          if (events[n].data.fd == sockfd)
            {
              conn_sock = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
              if (conn_sock == -1)
                {
                  WriteLogPError("accept");
                  exit(EXIT_FAILURE);
                }
              //setnonblocking(conn_sock);//if set non-blocking mode we need to use edge-triggered epoll mode and check EAGAIN in while loop to get all incoming data. it is asynchronous mode
              ev.events = EPOLLIN | EPOLLPRI;
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
              send_data_from_output_queue(output_queue);
              shutdown(events[n].data.fd, SHUT_RD); //shutdown is needed only if got a request for disconnecting
            }
        }
    }
}

int run_workers(int number_threads)
{
  // функция создает треды и распределяет работу
  int i;
  // треды смотрят очередь, просыпаются по сигналу, берут работу и выполняют ее(парсят запрос, ищут файл, готовят ответ, все это
  // помещают в структуру)
  // отправлять ответ в клиентский сокет

  for(i = 0;i < number_threads; i++)
    {
      //pthread_create();

    }

}

int create_job_with_raw_data_and_place_into_input_queue(jobs_queue_t* input_jobs_queue, int client_sock)
{
  job_t *job;
  size_t bytes_read;

  if((job = create_job()) == NULL)
    return -1;

  job->raw_data->client_socket=client_sock; //  copy fd to raw data structure
  bytes_read=read(job->raw_data->client_socket, job->raw_data->initial_data, INITIAL_DATA_SIZE); // reading data fron socket to structure
  job->raw_data->initial_data[bytes_read+1] = 0;//adds 0 to the end of data

  if(push_job(input_jobs_queue, job))
    {
      WriteLog("Error occured while pushing a job into jobs queue\n");
      return -1;
    }
  return 0;
}
// int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
// берем работу из одной очереди, делаем что-то и перекладываем в другую очередь(для ответа)

int process_jobs(jobs_queue_t* input, jobs_queue_t* output)
{
  job_t *job;
  int errsv;

  int fd;// file desctiptor of opened file

  if(pop_job(input, &job))
    {
      WriteLog("Error occured while popping up a job from jobs queue\n");
      return -1;

    }
  // парсим сырые данные и формируем запрос

  create_http_request_from_raw_data(job->req, job->raw_data);

  // находим файл и формируем ответ

  fd = open(job->req->params,O_RDONLY);

  if(fd < 0)
    {
      errsv = errno;
      if(errno == EACCES)
        {
          //
        }


    }




  //  помещаем в исходящую очередь

  if(push_job(output, job))
    {
      WriteLog("Error occured while pushing a job into jobs queue\n");
      return -1;
    }
}
size_t send_data_from_output_queue(jobs_queue_t* output_queue)
{

  job_t* job;

  push_job(output_queue, job);

  // отправляем ответ



  // deleting all allocated resources

  destroy_job(job);
}
