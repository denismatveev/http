#ifndef __WORKER_H__
#define __WORKER_H__

#include "jobs_queue.h"
#include "pthread.h"

typedef struct _worker
{
  job_t* job;
  pthread_t threadid;

} worker;

typedef worker* worker_t;

worker_t init_worker();
void delete_worker(worker_t);
void request_handle(*http_request_t);



















#endif //__WORKER_H__
