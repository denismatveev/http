#ifndef JOBS_QUEUE
#define JOBS_QUEUE
#include <malloc.h>
#include "http.h"
#include <pthread.h>
#define QUEUE_SIZE_RESERVE 102400


typedef struct __job
{
  http_request_old_t *req;
  raw_client_data_t *raw_data;
  http_response_old_t *response;
} job;

typedef job job_t;

typedef struct __jobs_queue
{
  pthread_mutex_t lock;
  char queuename[32];
  job_t **array;
  size_t capacity; // max number of elements can be placed in queue
  size_t high_bound;
  size_t low_bound;
  size_t size; // current number of elemnts in queue

} jobs_queue;
typedef jobs_queue jobs_queue_t;

// functions

job_t* create_job(void);
void destroy_job(job_t*);

jobs_queue_t* init_jobs_queue(const char *queuename);
int close_jobs_queue(jobs_queue_t*);

int push_job(jobs_queue_t*, job_t *);
int pop_job(jobs_queue_t *, job_t**);

#endif
