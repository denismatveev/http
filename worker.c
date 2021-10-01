#include "worker.h"
#include "common.h"
#include "http.h"
#include "config.h"
#include <pthread.h>
#include <stdlib.h>
#include "jobs_queue.h"
#include <errno.h>
#include <sys/sendfile.h>

// shared variables among threads
pthread_cond_t output_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t input_cond = PTHREAD_COND_INITIALIZER;
jobs_queue_t* input_queue;
jobs_queue_t* output_queue;
extern config_t *cfg;


int create_listener()
{

    int sockfd;
    struct sockaddr_in serv_addr;
    uint16_t port;
    int r=0;
    char addr_str[128];

    serv_addr.sin_family=AF_INET;

    port=(uint16_t)atoi(get_value(cfg->general->Set, general_reserved_names[0]));
    serv_addr.sin_port=htons(port);
    strncpy(addr_str, get_value(cfg->general->Set, general_reserved_names[1]), 128);

    if((r=inet_pton(AF_INET, addr_str, &serv_addr.sin_addr.s_addr)) == 1)
    {
        WriteLog("Listen to %s", addr_str);
    }
    else
    {
        WriteLog("Invalid IP address. Exit.");
        exit(EXIT_FAILURE);
    }

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

void run_server()
{
    //extern config_t *cfg;
    int conn_sock, nfds, epollfd, sockfd, err, ret;
    struct epoll_event ev, events[MAX_EVENTS];
    epollfd = epoll_create1(0);
    ev.events = EPOLLIN;// level triggered
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    int nworkers;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    sockfd=create_listener();

    // queues initializing
    input_queue = init_jobs_queue("input");
    output_queue = init_jobs_queue("output");

    nworkers=atoi(get_value(cfg->general->Set, general_reserved_names[2]));
    run_threads(nworkers,nworkers);
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
        for (int n = 0; n < nfds; ++n)
        {
            // event on listening socket
            if (events[n].data.fd == sockfd)
            {
                if((conn_sock = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) == -1)
                {
                    err=errno;
                    WriteLogPError("accept");
                    break;
                }
                // setnonblocking(conn_sock); // must be used in edge-triggered(ET) mode

                // ev.events = EPOLLIN | EPOLLET; // ET mode
                ev.events = EPOLLIN;
                ev.data.fd = conn_sock;
                if ((epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,&ev)) == -1)
                {
                    err=errno;
                    WriteLogPError("epoll_ctl");
                    break;
                }
            }
            else
                ret = create_job_to_process(events[n].data.fd);
        }

        pthread_mutex_lock(&mutex);
        if(input_queue->size > 0) //predicate to send a signal
            pthread_cond_broadcast(&input_cond); // new job appeared in input queue, sending a signal to wake up all threads
        pthread_mutex_unlock(&mutex);
    }

}

int create_job_to_process(int client_sock)
{
    job_t *job;
    ssize_t bytes_read;

#ifdef DEBUG
    WriteLog("Creating jobs");
#endif
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
    return 0;
}

// take job from one queue(input_queue), do something and put into another(output_queue)
void* process_jobs(void* args)
{
    //  struct sending_thread_args *targs = args;// you can pass args via this struct
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    job_t *job=NULL;
    int err;
    int ret;
    int fd; // opened file descriptor to be sent
    struct stat sb;
    while(1)
    {
        pthread_mutex_lock(&mutex);

        while(input_queue->size == 0)
            pthread_cond_wait(&input_cond,&mutex);

#ifdef DEBUG
        WriteLog("Processing job from input queue");
#endif

        if((pop_job(input_queue, &job)))
            continue;

        // parsing raw client data and create a http_request
        ret=process_http_data(job->req, job->raw_data);
        // depending on what data are in http_request we are making a response
        // TODO implement HEAD request
        if(ret < 0 )
        {
            // bad request
            create_400_response(job->response, job->req);
            fd=open("error_pages/400.html",O_RDONLY );
            job->response->message_body=fd;
            job->response->header.content_length_num=get_file_size(fd);
            convert_Content_Length(&job->response->header);
            goto PUSH;
        }
        // finding a file and creating a response
        if(job->req->method == GET)
        {
            fd = open(job->req->params, O_RDONLY);
            if((checkRegularFile(fd)) != 0)
            {
                create_404_response(job->response, job->req);
                fd=open("error_pages/404.html",O_RDONLY );
                job->response->message_body=fd;
                job->response->header.content_length_num=get_file_size(fd);
                convert_Content_Length(&job->response->header);
                goto PUSH;
            }

            if(fd > 0)
            {
                //everything is OK
                create_200_response(job->response, job->req);
                job->response->message_body=fd;
                job->response->header.content_length_num=get_file_size(fd);
                get_file_MIME_type_in_str(job->response->header.content_type, 256, job->req->params);
                convert_Content_Length(&job->response->header);
                goto PUSH;
            }
            else if(fd < 0)
            {
                //something went wrong
                err = errno;
                if(err == EACCES || err == ENOENT)
                {
                    //no such file
                    create_404_response(job->response, job->req);
                    fd=open("error_pages/404.html",O_RDONLY );
                    job->response->message_body=fd;
                    job->response->header.content_length_num=get_file_size(fd);
                    convert_Content_Length(&job->response->header);
                    goto PUSH;

                }
                else if(err == ENFILE || err == ELOOP)
                {
                    //specific errors
                    create_500_response(job->response, job->req);
                    fd=open("error_pages/500.html",O_RDONLY );
                    job->response->message_body=fd;
                    job->response->header.content_length_num=get_file_size(fd);
                    convert_Content_Length(&job->response->header);
                    goto PUSH;
                }
            }
        }
        else
        {
            // not implemented
            create_501_response(job->response, job->req);
            fd=open("error_pages/501.html",O_RDONLY );
            job->response->message_body=fd;
            job->response->header.content_length_num=get_file_size(fd);
            convert_Content_Length(&job->response->header);
            goto PUSH;
        }


PUSH:
        // pushing the job into output queue
        pthread_mutex_unlock(&mutex);
        if(push_job(output_queue, job))
        {
            WriteLog("Error occured while pushing a job into jobs queue\n");
            //return -1;
            continue;
        }

        pthread_mutex_lock(&mutex);
        if(output_queue->size > 0)
            pthread_cond_signal(&output_cond); // data placed in output queue
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}
void* send_data_to_client(void* args)
{

    //struct sending_thread_args *targs = args;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    job_t* job;
    char serialized_headers[512]={0};
    size_t headers_len;
    ssize_t sent_bytes=0;


    while(1)
    {
        pthread_mutex_lock(&mutex);// mutex here is needed to protect conditional variable
        while(output_queue->size == 0)
            pthread_cond_wait(&output_cond, &mutex);

#ifdef DEBUG
        WriteLog("Sending jobs");
#endif
        if((pop_job(output_queue, &job)))
            continue;

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

        pthread_mutex_unlock(&mutex);
    }
}

//TODO create function to close all allocated resources(queues, listening sockets) if signal came(i.e. to reload config)


int run_threads(int nprocessing, int nsenders)
{
    int rc;
    pthread_t *workers;
    pthread_t *senders;

    pthread_attr_t worker_attr;
    pthread_attr_t sender_attr;


    if((workers=(pthread_t*)calloc((size_t)nprocessing,sizeof(pthread_t))) == NULL)
        return -1;

    if((senders=(pthread_t*)calloc((size_t)nsenders,sizeof(pthread_t))) == NULL)
        return -1;

    if((rc=pthread_attr_init(&worker_attr)))
        return rc;
    if((rc=pthread_attr_setdetachstate(&worker_attr, PTHREAD_CREATE_DETACHED)))
        return rc;
    for(int i=0; i < nprocessing; ++i)
    {
        if((rc=pthread_create(workers + i, &worker_attr, process_jobs, NULL)))
        {
            WriteLogPError("creating processing thread");
            pthread_attr_destroy(&worker_attr);
            free(workers);

            return rc;
        }
    }

    if((rc=pthread_attr_init(&sender_attr)))
        return rc;
    if((rc=pthread_attr_setdetachstate(&sender_attr, PTHREAD_CREATE_DETACHED)))
        return rc;

    for(int i=0; i < nprocessing; ++i)
    {
        if((rc=pthread_create(senders + i, &sender_attr, send_data_to_client, NULL)))
        {
            WriteLogPError("creating sending thread");
            pthread_attr_destroy(&sender_attr);
            free(senders);

            return rc;
        }
    }
    return 0;
}
