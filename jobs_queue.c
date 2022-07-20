// This is the simpliest implementation of queue.
// thread safe

#include"jobs_queue.h"
#include "common.h"

job_t* create_job()
{
    job_t* j;

    if((j =(job_t*)malloc(sizeof(job_t))) == NULL)
        return NULL;
    if((j->req = init_http_request()) == NULL)
        return NULL;
    if((j->response = init_http_response()) == NULL)
        return NULL;
    j->socket=0;
    j->fd=0;
    return j;

}

void destroy_job(job_t* j)
{
    if(j == NULL)
    {
        WriteLog("Attempt to delete a job which does not exist");
        return;
    }
    delete_http_request(j->req);
    delete_http_response(j->response);
    close(j->socket);
    close(j->fd);
    free(j);

}


jobs_queue_t* init_jobs_queue(const char *queuename)
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
    q->capacity = QUEUE_SIZE_RESERVE;
    strncpy(q->queuename,queuename, 32);
    q->size=q->high_bound-q->low_bound;

    if((pthread_mutex_init(&q->lock,NULL)))
        return NULL;

    return q;

}
int close_jobs_queue(jobs_queue_t *q)
{
    if(q == NULL)
    {
        WriteLog("Deleting queue which does not exist!");
        return -1;
    }
    size_t i;
    for(i=q->low_bound; i < q->high_bound; i++)
    {
        destroy_job(*(q->array+i));
    }

    q->low_bound=q->high_bound = 0;
    q->capacity = 0;
    pthread_mutex_destroy(&q->lock);
    free(q->array);
    free(q);

    return 0;
}

int push_job(jobs_queue_t *q, job_t *j)
{

    pthread_mutex_lock(&q->lock);
    job_t **ptr;

    if(q->high_bound < q->capacity)
    {
        q->array[(q->high_bound)++]=j;
#ifdef QUEUEDEBUG
        WriteLog("Pushed job into queue '%s'", q->queuename);
#endif
        q->size=q->high_bound-q->low_bound;
        pthread_mutex_unlock(&q->lock);
        return 0;
    }
    else if(q->high_bound == q->capacity) //queue is full, doing reallocation
    {
        ptr=(job_t**)realloc(q->array,(q->capacity)*sizeof(job_t*) + QUEUE_SIZE_RESERVE*sizeof(job_t*));

        if(ptr == NULL)
        {
            WriteLogPError("push_job()");
            pthread_mutex_unlock(&q->lock);
            return 2;
        }
        q->array = ptr;
        q->array[(q->high_bound)++]=j;
        q->capacity += QUEUE_SIZE_RESERVE;

#ifdef QUEUEDEBUG
        WriteLog("Pushed job into queue '%s'", q->queuename);
#endif
        q->size=q->high_bound-q->low_bound;
        pthread_mutex_unlock(&q->lock);
        return 0;
    }
    q->size=q->high_bound-q->low_bound;
    pthread_mutex_unlock(&q->lock);

    return 1;
}
int pop_job(jobs_queue_t* q, job_t **j)
{
    pthread_mutex_lock(&q->lock);
    size_t size, i;
    job_t** ptr;

    if(q->low_bound == q->high_bound)
    {
        q->low_bound = q->high_bound = 0;
        *j = NULL;
#ifdef QUEUEDEBUG
        WriteLog("Queue '%s' is empty", q->queuename);
#endif
        q->size=q->high_bound-q->low_bound;
        pthread_mutex_unlock(&q->lock);

        return 3;//queue is empty
    }
    else if(q->low_bound < QUEUE_SIZE_RESERVE)
    {
        *j = q->array[(q->low_bound)++];
#ifdef QUEUEDEBUG
        WriteLog("Popped from queue '%s'", q->queuename);
#endif
        q->size=q->high_bound-q->low_bound;
        pthread_mutex_unlock(&q->lock);
        return 0;
    }
    else if(q->low_bound == QUEUE_SIZE_RESERVE)
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
                pthread_mutex_unlock(&q->lock);
                return 2;
            }
            q->array = ptr;
            q->capacity=QUEUE_SIZE_RESERVE;
        }
        q->high_bound=size;//set high_bound to size
        *j = q->array[(q->low_bound)++];
#ifdef QUEUEDEBUG
        WriteLog("Popped from queue '%s'", q->queuename);
#endif
        q->size=q->high_bound-q->low_bound;
        pthread_mutex_unlock(&q->lock);
        return 0;
    }

    pthread_mutex_unlock(&q->lock);
    return 1;

}


