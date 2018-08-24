//TODO web server logs to write
//TODO multithread or multiprocess
//TODO config
//TODO parse request and make response

#include"http.h"
#include "parse_config.h"
#include "listener.h"
#include "ioworker.h"
#include "common.h"
#include "jobs_queue.h"
#include <pthread.h>

//global jobs queue
jobs_queue_t* jobs;

int main(int argc, char** argv)
{
  char *cfgFile = NULL;
  char *cfgFilePath="/etc/swd/swd.conf";//default config file name
  int sock;

  pid_t pid;
  char c;
  config_t cfg;

  while ((c = getopt (argc, argv, ":c:")) != -1)
  {
    switch (c)
    {
    case 'c':
      cfgFile = optarg;
      break;
    case '?':
      fprintf(stderr, "Unknown option -%c\n", optopt);
      exit(EXIT_FAILURE);
    case ':':
      fprintf(stderr, "Option -%c requires an argument\n", optopt);
      exit(EXIT_FAILURE);
    default:
      exit(EXIT_FAILURE);
    }
  }
  if(cfgFile == NULL)
    cfgFile=cfgFilePath;


  if((parse_cfg(&cfg,cfgFile) == -1))
  {
    WriteLog("Using default config\n");
    default_cfg(&cfg);
  }



  pid=fork();//child process

  if(pid == -1)
  {
    fprintf(stderr, "Error starting daemon %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
  if(pid)
  {
    fprintf(stderr,"[Daemon] Started OK, My PID = %i\n", pid);
    exit(EXIT_SUCCESS);
  }
  /* the following code is executing in child process */
  if((setsid()) < 0)
  {
    fprintf(stderr,"[Daemon] An Error occured. Stop\n");
    exit(EXIT_FAILURE);
  }
  umask(0);

  if((chdir("/")) < 0)
  {
    fprintf(stderr,"[Daemon] Can't change directory\n");
    exit(EXIT_FAILURE);
  }

  fclose(stderr);
  fclose(stdin);
  fclose(stdout);

  jobs=init_jobs_queue();
  create_listener();

  // TODO create workers
  // worker is a thread that waits for some data in a the queue, by signal of cond variable takes a job from the queue and send answer to client

  close_jobs_queue(jobs);

  close(sock);

  return 0;
}

