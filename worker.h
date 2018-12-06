#ifndef IOWORKER_H
#define IOWORKER_H
#include "jobs_queue.h"
#include "parse_config.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdlib.h>
#define MAX_EVENTS 10
int create_listener(void);
void create_worker(int sock);

int create_job_with_raw_data_and_place_into_input_queue(jobs_queue_t*, int);
int process_jobs(jobs_queue_t* input, jobs_queue_t* output);
ssize_t send_data_from_output_queue(jobs_queue_t* output_queue);
int run_workers(int number_threads);

extern jobs_queue_t* input_queue;
extern jobs_queue_t* output_queue;

#endif //IOWORKER_H
