#ifndef QUEUE_H
#define QUEUE_H
#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include"config.h"

#define ALLOCATION_ERROR 3

typedef struct q_elem
{
    node_t* node;
    char is_end;
    struct q_elem *next;
}__q_elem;
typedef __q_elem* q_elem_t;
typedef struct _Queue
{
    q_elem_t first;
    q_elem_t last;
}__Queue;

typedef __Queue* Queue;
Queue init_queue(void);
int push(Queue, q_elem_t);
int pop(Queue, q_elem_t*);
q_elem_t init_q_elem(node_t*);
void destroy_q_elem(q_elem_t);
void destroy_queue(Queue);
#endif
