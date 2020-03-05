#ifndef QUEUE_TEST
#define QUEUE_TEST
#include "jobs_queue.h"
#include<pthread.h>
//#include "cmockery.h"
#define QUEUES 5
#define THREADS 15
#define NJOBS 3000

struct thread_args
{
  int n_queues;
  int n_jobs;

};


struct thread_info
{
   pthread_t threads[THREADS];
   pthread_attr_t attr[THREADS];
};

#endif // QUEUE_TEST
