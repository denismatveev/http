#ifndef IOWORKER_H
#define IOWORKER_H
#include "jobs_queue.h"
#include "config.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdlib.h>
#define MAX_EVENTS 10

extern jobs_queue_t* input_queue;
extern jobs_queue_t* output_queue;
extern pthread_cond_t output_cond;
extern pthread_cond_t input_cond;

struct processing_threads_info {    /* Used as argument to thread_start() */
  pthread_t thread_id;        /* ID returned by pthread_create() */
  int       thread_num;       /* Application-defined thread # */
};

typedef struct sending_thread_args
{
  pthread_mutex_t mutex;
} sending_thread_args_t;

typedef struct processing_thread_args
{
  pthread_mutex_t mutex;
} processing_thread_args_t;
int create_listener(void);
void create_worker(void) __attribute__ ((noreturn)) ;

int create_job_with_raw_data_and_place_into_input_queue(int);
void* process_jobs(void*);
void *send_data_from_output_queue(void*);

int run_threads(int nprocessing_threads, int nsender_threads);
#endif //IOWORKER_H
