#include"queue.h"

int main(int argc, char ** argv)
{

    node_t* n;
    q_elem_t qe;
    Queue q = init_queue();

    n=init_node("1","/var/","index.html");
    qe=init_q_elem(n);
    push(q,qe);

    n=init_node("2","/var/","index.html");
    qe=init_q_elem(n);
    push(q,qe);

    n=init_node("host 3","/var/","index.html");
    qe=init_q_elem(n);
    push(q,qe);



    if(!pop(q,&qe))
    {
        printf("name=%s\n", qe->node->hostname);
        destroy_q_elem(qe);
    }

    if(!pop(q,&qe))
    {
        printf("name=%s\n", qe->node->hostname);
        destroy_q_elem(qe);
    }

        n=init_node("4","/var/","index.html");
        qe=init_q_elem(n);
        push(q,qe);

        if(!pop(q,&qe))
        {
            printf("name=%s\n", qe->node->hostname);
            destroy_q_elem(qe);
        }



        if(!pop(q,&qe))
        {
            printf("name=%s\n", qe->node->hostname);
            destroy_q_elem(qe);
        }
    destroy_queue(q);




}
