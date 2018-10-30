//TODO web server logs to write
//TODO multithread or multiprocess
//TODO config
//TODO parse request and make response

#include"http.h"
#include "parse_config.h"
#include "listener.h"
#include "worker.h"
#include "common.h"
#include <pthread.h>

char *cfgFile = NULL;
char *cfgFilePath="/etc/swd/swd.conf";//default config file name

int main(int argc, char** argv)
{

  int sock;
  int i;

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

  sock=create_listener();
  create_ioworker(sock);




  close(sock);

  return 0;
}

