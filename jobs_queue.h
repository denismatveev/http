#ifndef JOBS_QUEUE
#define JOBS_QUEUE
#include <malloc.h>
#include "http.h"
#include <pthread.h>
#define QUEUE_SIZE_RESERVE 102400


typedef struct __job
{
  http_request_t *req;
  raw_client_data_t *raw_data;
} job;

typedef job job_t;

typedef struct __jobs_queue
{
  pthread_mutex_t *mtr;
  job_t **array;
  size_t array_size;
  size_t high_bound;
  size_t low_bound;

} jobs_queue;
typedef jobs_queue jobs_queue_t;

// functions

job_t* create_job(http_request_t *, raw_client_data_t *);
void destroy_job(job_t*);

jobs_queue_t* init_jobs_queue();
int close_jobs_queue(jobs_queue_t*);

int push_job(jobs_queue_t*, job_t *);
int pop_job(jobs_queue_t*, job_t**);



#endif
