#include"queue.h"
#include"common.h"

q_elem_t init_q_elem(node_t* node)
{
    q_elem_t q;
    if((q=(q_elem_t)malloc(sizeof (__q_elem))) == NULL)
    {
        WriteLogPError("Initializing q_elem_t:");
        return NULL;
    }

    q->next=NULL;
    q->node=node;
    q->is_end = 0;

    return q;
}

void destroy_q_elem(q_elem_t qe)
{
    if(qe == NULL)
        return;
    destroy_node(qe->node);
    free(qe);
    return;
}

Queue init_queue(void)
{
    Queue Q;

    if((Q=(Queue)malloc(sizeof (__Queue))) == NULL)
    {
        WriteLogPError("Initializing queue in red-black tree");
        return NULL;
    }

    Q->last=NULL;
    Q->first=NULL;

    return Q;
}

void destroy_queue(Queue q)
{
    q_elem_t tmp;
    if(q == NULL)
        return;

    while(q->first != NULL && q->last != NULL)
    {
        tmp=q->first->next;
        destroy_q_elem(q->first);
        q->first=tmp;
    }

    free(q);
    return;
}

int push(Queue q, q_elem_t qe)
{
    if(q->last == NULL)
    {
        q->first=q->last=qe;
        qe->next=NULL;
    }
    else
    {
        q->last->next=qe;
        q->last=qe;
    }

    return 0;
}

int pop(Queue q, q_elem_t* qe)
{
    if(q->first != NULL)
    {
        if(q->first != q->last)
        {
            *qe=q->first;
            q->first=q->first->next;
            (*qe)->next=NULL;
        }
        else
        {
            *qe=q->first;
            q->first=q->last=NULL;
        }
        return 0;
    }

    return 1;
}

