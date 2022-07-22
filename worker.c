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

int create_listener()
{

    int sockfd;
    struct sockaddr_in serv_addr;
    uint16_t port;
    int r=0;
    char addr_str[128];

    serv_addr.sin_family=AF_INET;

    port=(uint16_t)atoi(get_value(cfg->general->Set,general_reserved_names[0] ));
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
    int conn_sock, nfds, epollfd, sockfd, err, ret;
    struct epoll_event ev, events[MAX_EVENTS];
    epollfd = epoll_create1(0);
    ev.events = EPOLLIN;// level triggered, register only data avaialbale for reading
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    int nworkers;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    sockfd=create_listener();

    // queues initializing
    input_queue = init_jobs_queue("input");
    output_queue = init_jobs_queue("output");

    nworkers=atoi(get_value(cfg->general->Set, general_reserved_names[2]));
    if(nworkers < 1)
    {
        WriteLog("Number of workers myst be greater 0");
        exit(1);
    }
    run_threads(nworkers);
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
    //TODO the loop below to be placed into dedicated thread
    while (1)
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
            {
#ifdef DEBUG
                WriteLog("New event on socket %d", events[n].data.fd);
#endif
                ret = create_job_to_process(events[n].data.fd);
            }
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

    job->socket=client_sock;
    bytes_read=read(client_sock, job->raw_data, INITIAL_DATA_SIZE); // reading data from the socket to structure
    job->raw_data[bytes_read+1] = 0;//adds 0 to the end of data
#ifdef DEBUG
    WriteLog("Raw data: %s", job->raw_data);
#endif

    if((push_job(input_queue, job)))
    {
        WriteLog("Error occured while pushing a job into jobs queue\n");
        destroy_job(job);
        return -1;
    }

#ifdef DEBUG
    WriteLog("push job %p in input queue by thread %lu in process() function", job, pthread_self());
#endif
    return 0;
}

// take job from one queue(input_queue), do something and put into another(output_queue)
void* process_jobs(void* args)
{
    //  struct sending_thread_args *targs = args;// you can pass args via this struct
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    char* rootdir;

    job_t *job;
    http_header_node_t* header_node;
    section_t cfg_section;

    int err;
    int ret;
    char date[DATE_HEADER_MAX_LENGTH];
    char * indexfile;

    while(1)
    {
        header_node = NULL;
        job = NULL;

        pthread_mutex_lock(&mutex);
        while(input_queue->size == 0)
            pthread_cond_wait(&input_cond,&mutex);
        pthread_mutex_unlock(&mutex);


#ifdef DEBUG
        WriteLog("Processing job from input queue in thread %lu queue size is %d", pthread_self(), input_queue->size);
#endif

        if((pop_job(input_queue, &job)))
            continue;
#ifdef DEBUG
        WriteLog("pop job %p from input queue by thread %lu in process() function", job, pthread_self());
#endif
        // parsing raw client data and create a http_request
        ret=process_http_data(job->req, job->raw_data);
        // depending on which data are in http_request we are making a response

        if(ret == 0)
        {
#ifdef DEBUG
            if(job->req->parsing_result == 0)
            {

                char proto[HTTP_PROTOCOL_VERSION_MAX_LENGTH],method[16];
                http_header_node_t* node;
                char header_name[HTTP_HEADER_NAME_MAX_LEN], header_value[HTTP_HEADER_VALUE_MAX_LEN];
                http_method_to_str(job->req->req_line.method, method, 16);
                http_ptorocol_code_to_str(proto, job->req->req_line.http_version,HTTP_PROTOCOL_VERSION_MAX_LENGTH);
                WriteLog("Request Line: %s %s %s", method, job->req->req_line.request_URI, proto);
                WriteLog("Headers:");

                node=job->req->headers->first;

                while(node != NULL)
                {
                    header_name_to_str_value_by_type(node, header_name, header_value);
                    WriteLog("%s %s",header_name, header_value);
                    node=node->next;
                }
            }

#endif
            if(job->req->parsing_result == 0)
            {
                switch(job->req->req_line.method)
                {
                case HTTP_METHOD_HEAD:

                    // looking for Host header in a request and then search if there is such host in cfg, otherwise use default host
                    if((header_node=find_header_by_type(NULL, job->req, http_request_header, REQUEST_HEADER_HOST)) != NULL)
                        cfg_section=search_for_host_in_cfg(cfg, header_node->http_header_value);
                    else
                        cfg_section=cfg->default_vhost;

                    rootdir=get_value(cfg_section->Set,"rootdir");
                    get_current_date_str(date);

                    if(chdir(rootdir))
                    {
                        WriteLogPError("Requested file");
                        create_error_message(job->response, REASON_FORBIDDEN, NO_MSG_BODY);
                    }
                    if(job->req->req_line.request_URI[0] == '/' && job->req->req_line.request_URI[1] == 0)
                    {
                        indexfile=get_value(cfg_section->Set,"indexfilepath");
                        job->fd=open(indexfile, O_RDONLY);// file descriptor will be closed in destroy_job()
                        set_reason_code(job->response, 200);
                        add_file_as_message_body(job->response, job->fd, indexfile, NO_MSG_BODY);
                    }
                    else
                    {
                        if((job->fd=open(job->req->req_line.request_URI+1, O_RDONLY | O_NOFOLLOW)) > 0)
                        {

                            if((checkRegularFile(job->fd)) == 0)
                            {
                                set_reason_code(job->response, 200);
                                add_file_as_message_body(job->response, job->fd, job->req->req_line.request_URI+1, NO_MSG_BODY);
                            }
                            else
                                create_error_message(job->response, REASON_BAD_REQUEST, NO_MSG_BODY);
                        }
                        else
                        {
                            err=errno;
                            if(err == EACCES)
                                create_error_message(job->response, REASON_FORBIDDEN, NO_MSG_BODY);
                            else if( err == ENOENT)
                                create_error_message(job->response, REASON_NOT_FOUND, NO_MSG_BODY);
                            else
                                create_error_message(job->response, REASON_INTERNAL_ERROR, NO_MSG_BODY);
                        }
                    }

                    add_header_to_response(job->response, "Date", date);
                    add_header_to_response(job->response, "Server", SERVERNAME);

                    goto PUSH;

                case HTTP_METHOD_GET:

                    if((header_node=find_header_by_type(NULL, job->req, http_request_header, REQUEST_HEADER_HOST)) != NULL)
                        cfg_section=search_for_host_in_cfg(cfg, header_node->http_header_value);
                    else
                        cfg_section=cfg->default_vhost;
                    rootdir=get_value(cfg_section->Set,"rootdir");
                    get_current_date_str(date);
                    if(chdir(rootdir))
                    {
                        WriteLogPError("Requested file");
                        create_error_message(job->response, REASON_FORBIDDEN, NO_MSG_BODY);
                    }

                    if(job->req->req_line.request_URI[0] == '/' && job->req->req_line.request_URI[1] == 0)
                    {
                        indexfile=get_value(cfg_section->Set,"indexfilepath");
                        job->fd=open(indexfile, O_RDONLY);// file descriptor will be closed in destroy_job()
                        set_reason_code(job->response, 200);
                        add_file_as_message_body(job->response, job->fd, indexfile, SET_MSG_BODY);
                    }
                    else
                    {
                        if((job->fd=open(job->req->req_line.request_URI+1, O_RDONLY | O_NOFOLLOW)) > 0)
                        {
                            if((checkRegularFile(job->fd)) == 0)
                            {
                                set_reason_code(job->response, 200);
                                add_file_as_message_body(job->response, job->fd, job->req->req_line.request_URI+1, SET_MSG_BODY);
                            }
                            else
                                create_error_message(job->response, REASON_BAD_REQUEST, SET_MSG_BODY);
                        }
                        else
                        {
                            err=errno;
                            if(err == EACCES)
                                create_error_message(job->response, REASON_FORBIDDEN, SET_MSG_BODY);
                            else if( err == ENOENT)
                                create_error_message(job->response, REASON_NOT_FOUND, SET_MSG_BODY);
                            else
                                create_error_message(job->response, REASON_INTERNAL_ERROR, SET_MSG_BODY);
                        }
                    }
                    add_header_to_response(job->response, "Date", date);
                    add_header_to_response(job->response, "Server", SERVERNAME);
                    goto PUSH;
                default:
                    get_current_date_str(date);
                    add_header_to_response(job->response, "Date", date);
                    add_header_to_response(job->response, "Server", SERVERNAME);
                    create_error_message(job->response, REASON_NOT_IMPLEMENTED, SET_MSG_BODY);
                    goto PUSH;
                }
            }
            else if(job->req->parsing_result == 1)
            {
                WriteLog("Cannot parse request: %s", job->raw_data);
                get_current_date_str(date);
                add_header_to_response(job->response, "Date", date);
                add_header_to_response(job->response, "Server", SERVERNAME);
                create_error_message(job->response, REASON_BAD_REQUEST, SET_MSG_BODY);
                goto PUSH;
            }
        }


        if(ret != 0 )
        {
            WriteLog("Something went wrong while processing http request, check logs");
            return NULL;

        }

PUSH:
        // pushing the job into output queue
        if(push_job(output_queue, job))
        {
            WriteLog("Error occured while pushing a job into jobs queue");
            continue;
        }

#ifdef DEBUG
        WriteLog("push job %p in output queue by thread %lu in process() function", job, pthread_self());
#endif
        pthread_mutex_lock(&mutex);
        if(output_queue->size > 0)
            pthread_cond_signal(&output_cond); // data placed in output queue
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}
void* send_data_to_client(void* args)
{

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    job_t* job;
    char serialized_data[10000];// this is max len of the longest header name plus HTTP_HEADER_VALUE_MAX_LEN multiplied by max number of headers(64)
    size_t headers_len=0;
    ssize_t sent_bytes=0;

    while(1)
    {
        pthread_mutex_lock(&mutex);// mutex here is needed to protect conditional variable
        while(output_queue->size == 0)
            pthread_cond_wait(&output_cond, &mutex);
        pthread_mutex_unlock(&mutex);



#ifdef DEBUG
        WriteLog("Sending jobs");
#endif
        if((pop_job(output_queue, &job)))
            continue;

#ifdef DEBUG
        WriteLog("pop job %p from output queue by thread %lu in send() function", job, pthread_self());
#endif
        memset(serialized_data,0, 10000);// make sense to move these two functions to process_jobs() and parallelize the work, and serialized_data put to job struct
        process_http_response(serialized_data,job->response, 10000);
        // TODO everywhere replace C string by string_t to prevent counting length each time(it is too many times!) and spend CPU
        headers_len=strlen(serialized_data);
        // sending a response
        // At first we need to send headers
        sent_bytes = write(job->socket, serialized_data, headers_len);
        // then we need to send either file or buffer
        if(job->response->message_body_size > 0 && job->response->error_message == NULL)
        {
            if(job->fd > 0)
                sent_bytes += sendfile(job->socket,job->fd,NULL, job->response->message_body_size);
            else
                sent_bytes += write(job->socket, job->response->message_body, job->response->message_body_size);
        }
        else if (job->response->message_body_size > 0)
            sent_bytes += write(job->socket, job->response->error_message, job->response->message_body_size);

        // deleting all allocated resources
        destroy_job(job);

    }
}

//TODO create function to close all allocated resources(queues, listening sockets) or save somewhere them if signal came(i.e. to reload config)


int run_threads(int nprocessing)
{
    int rc;
    pthread_t *workers;
    pthread_t sender;

    pthread_attr_t worker_attr;
    pthread_attr_t sender_attr;


    if((workers=(pthread_t*)calloc((size_t)nprocessing,sizeof(pthread_t))) == NULL)
        return -1;

    if((rc=pthread_attr_init(&worker_attr)))
    {
        free(workers);
        return rc;
    }
    if((rc=pthread_attr_setdetachstate(&worker_attr, PTHREAD_CREATE_DETACHED)))
    {
        pthread_attr_destroy(&worker_attr);
        free(workers);
        return rc;
    }
    for(int i=0; i < nprocessing; ++i)
    {
        if((rc=pthread_create(workers + i, &worker_attr, process_jobs, NULL)))
        {
            WriteLogPError("creating processing thread");
            pthread_attr_destroy(&worker_attr);

            return rc;
        }
    }

    if((rc=pthread_attr_init(&sender_attr)))
        return rc;
    if((rc=pthread_attr_setdetachstate(&sender_attr, PTHREAD_CREATE_DETACHED)))
    {
        pthread_attr_destroy(&sender_attr);
        return rc;

    }

    if((rc=pthread_create(&sender, &sender_attr, send_data_to_client, NULL)))
    {
        WriteLogPError("creating sending thread");
        pthread_attr_destroy(&sender_attr);

        return rc;
    }
    return 0;
}
