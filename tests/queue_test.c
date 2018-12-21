#include "queue_test.h"

jobs_queue_t* q_array[QUEUES];
pthread_barrier_t barrier;
void* run_threads(void* args)
{

  struct thread_args *targs = args;

  int jn = 0;
  char* str="GET /test.html HTTP/1.1";
  job_t* j;
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

  pthread_t selfid;
  selfid=pthread_self();
  // resulting number of jobs are QUEUES*NJOBS
  for(int i = 0; i < NJOBS; i++)
    {
      //  printf("thread id=%lu\n", selfid);
      for(int k = 0; k < QUEUES; k++,jn++)
        {
          j=create_job();// jobs cannot be used in different queues, because closing queues causes jobs deleting
          strncpy(j->raw_data->initial_data, str, 100);
          push_job(q_array[k], j);
          pop_job(q_array[k],&j);
        }
    }

  pthread_mutex_lock(&lock);
  printf("Stats: \n\tpthread_id=%lu\n\tjobs:%d\n",selfid,jn);
  pthread_mutex_unlock(&lock);
  pthread_barrier_wait(&barrier);
  pthread_exit(0);

}

int main(int argc, char** argv)
{

  struct thread_info tinfo;
  struct thread_args targs;
  targs.n_queues=QUEUES;
  targs.n_jobs=NJOBS;
  char qname[32];
  char qn[12];

  pthread_barrier_init(&barrier, NULL, THREADS+1);

  int rc;
  strncpy(qname, "input queue ", 32);
  for(int i = 0; i < QUEUES;i++)
    {
      printf("initing queue: %d\n", i);
      int_to_str(qn, i);
      strncat(qname, qn, 2);


      q_array[i] = init_jobs_queue(qname);
    }


  for(int j = 0; j < THREADS; j++)
    {
      if((rc=pthread_attr_init(&tinfo.attr[j])))
        return rc;

      if((rc=pthread_attr_setdetachstate(&tinfo.attr[j], PTHREAD_CREATE_DETACHED)))
        return rc;

      pthread_create(&tinfo.threads[j], &tinfo.attr[j], run_threads, &targs);
    }

  pthread_barrier_wait(&barrier);

  for(int i = 0; i < QUEUES; i++)
    {
      printf("Closing queue: %d\n", i);
      close_jobs_queue(q_array[i]);
    }
  pthread_barrier_destroy(&barrier);
}
