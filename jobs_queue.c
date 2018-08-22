// This is the simpliest implementation of queue.
// thread safe

#include"jobs_queue.h"
#include "common.h"

job_t* create_job(http_request_t *ht)
{
  job_t* j;
  if((j =(job_t*)malloc(sizeof(job_t))) == NULL)
    return NULL;
  j->req=ht;

  return j;

}

void destroy_job(job_t* j)
{
  free(j);
}


jobs_queue_t* init_jobs_queue()
{

  jobs_queue_t *q;
  if((q=(jobs_queue_t*)malloc(sizeof(jobs_queue_t))) == NULL)
    {
      WriteLogPError("init_jobs_queue()");
      return NULL;
    }
  if((q->array=(job_t**)calloc(QUEUE_SIZE_RESERVE,sizeof(job_t*))) == NULL)
    {
      WriteLogPError("init_jobs_queue()");
      return NULL;
    }
  if((pthread_mutex_init(q->mtr, NULL)) != 0)
    {
      WriteLogPError("pthread_mutex_init()");
      return NULL;
    }

  q->high_bound=q->low_bound = 0;
  q->array_size = QUEUE_SIZE_RESERVE;

  return q;

}
int close_jobs_queue(jobs_queue_t *q)
{
  q->low_bound=q->high_bound = 0;
  q->array_size = 0;
  free(q->array);
  pthread_mutex_destroy(q->mtr);
  free(q);

  return 0;
}

int push_job(jobs_queue_t *q, job_t *j)
{

  job_t **ptr;

  if(q->high_bound < q->array_size)
    {
      pthread_mutex_lock(q->mtr);
      q->array[(q->high_bound)++]=j;
      pthread_mutex_unlock(q->mtr);

      return 0;
    }
  else if(q->high_bound == q->array_size) //queue is full
    {
      pthread_mutex_lock(q->mtr);
      ptr=(job_t**)realloc(q->array,(q->array_size)*sizeof(job_t*) + QUEUE_SIZE_RESERVE*sizeof(job_t*));

      if(ptr == NULL)
        {
          WriteLogPError("push_job()");
          return 2;
        }
      q->array = ptr;
      q->array[(q->high_bound)++]=j;
      q->array_size += QUEUE_SIZE_RESERVE;
      pthread_mutex_unlock(q->mtr);

      return 0;
    }
  return 1;
}
int pop_job(jobs_queue_t* q, job_t **j)
{
  unsigned int size, i;
  job_t** ptr;

  if(q->low_bound == q->high_bound)
    {
      pthread_mutex_lock(q->mtr);
      q->low_bound = q->high_bound = 0;
      *j = NULL;
      pthread_mutex_unlock(q->mtr);
      return 3;//queue is empty
    }
  else if(q->low_bound < QUEUE_SIZE_RESERVE)
    {
      pthread_mutex_lock(q->mtr);
      *j = q->array[(q->low_bound)++];
      pthread_mutex_unlock(q->mtr);
      return 0;
    }
  else if(q->low_bound = QUEUE_SIZE_RESERVE)
    {
      pthread_mutex_lock(q->mtr);
      size = (q->high_bound) - (q->low_bound);//size of the queue
      //moving all data to zero level and reset the low_bound to null
      for(i=0; i < size; i++)
        q->array[i]=q->array[(q->low_bound)++];
      q->low_bound = 0;//reset low_bound
      if(size < QUEUE_SIZE_RESERVE)
        {
          ptr=((job_t**)realloc(q->array,QUEUE_SIZE_RESERVE*sizeof(job_t*)));
          if(ptr == NULL)
            {
              WriteLogPError("pop_job()");
              return 2;
            }
          q->array = ptr;
          q->array_size=QUEUE_SIZE_RESERVE;
        }
      q->high_bound=size;//set high_bound to size

      *j = q->array[(q->low_bound)++];
      pthread_mutex_unlock(q->mtr);
      return 0;
    }

  return 1;

}
