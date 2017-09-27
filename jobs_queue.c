#include"jobs_queue.h"

jobs_queue_t init_jobs_queue()
{
  jobs_queue_t jqt;
  if((jqt=(jobs_queue_t)calloc(sizeof(jobs_queue))) == NULL)
  {  
     WriteLogPError("init_jobs_queue()");
     return NULL;
  }
  

 




}
int close_jobs_queue(jobs_queue_t);

int push_job(jobs_queue_t, job_t*);
job_t* pop_job(jobs_queue_t);
