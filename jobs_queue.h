#ifndef JOBS_QUEUE
#define JOBS_QUEUE
#include <malloc.h>
#define QUEUE_SIZE_RESERVE 1024
typedef struct __job
{
    *http_request_t req;
} job;

typedef job job_t;

typedef struct __jobs_queue
{
   size_t queue_size;
   job_t *array;
   size_t array_size;
   size_t high_bound;
   size_t low_bound;

} jobs_queue;
typedef jobs_queue* jobs_queue_t;
jobs_queue_t init_jobs_queue();
int close_jobs_queue(jobs_queue_t);

int push_job(jobs_queue_t, job_t*);
job_t* pop_job(jobs_queue_t);


#endif
