#ifndef JOBS_QUEUE
#define JOBS_QUEUE
#include <malloc.h>
#define QUEUE_SIZE_RESERVE 1024
typedef struct __job
{



} __job*;

typdef __job* job;


typedef struct __jobs_queue
{
   unsigned int size;
   job *array;
   unsigned int high_bound;
   unsigned int low_bound;

} jobs_queue;
typedef jobs_queue* jobs_queue;
jobs_queue init_jobs_queue();
int close_jobs_queue(jobs_queue);

int push_request(jobs_queue, job);
job pop_request(jobs_queue);


#endif