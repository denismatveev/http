#include"jobs_queue.h"

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
  
  q->high_bound=q->low_bound = 0;
  q->array_size = QUEUE_SIZE_RESERVE;

  return q;

}
int close_jobs_queue(jobs_queue_t *q)
{
  q->low_bound=q->high_bound = 0;
  q->array_size = 0;
  free(q->array);
  free(q);

  return 0;
}

int push_job(jobs_queue_t *q, job_t *j)
{
  job_t **ptr;
  unsigned int i, size;

  if(q->high_bound < q->array_size)
    {
      q->array[(q->high_bound)++]=j;

      return 0;
    }
  else if(q->high_bound == q->array_size && q->low_bound == 0) //queue is full
    {
      ptr=(job_t**)realloc(q->array,(q->array_size)*sizeof(job_t*) + QUEUE_SIZE_RESERVE*sizeof(job_t*));

      if(ptr == NULL)
        {
          WriteLogPError("push_job()");
          return 2;
        }
      q->array = ptr;
      q->array[(q->high_bound)++]=j;
      q->array_size += QUEUE_SIZE_RESERVE;

      return 0;
    }
  else if(q->high_bound == q->array_size && q->low_bound > 0) //if we don't have place to push but we have empty cells in the beginning, we should move elements to the beginning
    {
      size = q->high_bound - q->low_bound;
      for(i = 0;i < size;i++)
        q->array[i] = q->array[(q->low_bound++)];
      q->low_bound = 0;
      q->high_bound = size;
      q->array[(q->high_bound)++]=j;

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
      q->low_bound = q->high_bound = 0;
      *j = NULL;
      return 3;//queue is empty
    }


  else if(q->low_bound < QUEUE_SIZE_RESERVE)
    {
      *j = q->array[(q->low_bound)++];

      return 0;
    }
  else if(q->low_bound >= QUEUE_SIZE_RESERVE)
    {
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
      return 0;
    }
  return 1;

}
//TODO make all possible checks in pop() and push() actions
