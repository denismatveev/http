#include "http.h"
#include "parse_config.h"
#include "worker.h"
#include "common.h"
#include <pthread.h>
#include <signal.h>
static char *cfgFile = NULL;
static char *cfgFilePath="/etc/swd/swd.cnf"; // default config file name
config_t cfg;
int main(int argc, char** argv)
{

  pid_t pid;
  int c;
  sigset_t set;

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
      WriteLog("Using default config");
      default_cfg(&cfg);
    }

  pid=fork();//child process

  if(pid == -1)
    {
      WriteLogPError("Error starting daemon");
      exit(EXIT_FAILURE);
    }
  if(pid)
    {
      WriteLog("Started OK, My PID = %i", pid);
      exit(EXIT_SUCCESS);
    }
  /* the following code is executing in child process */
  if((setsid()) < 0)
    {
      WriteLog("An Error occured. Stop");
      exit(EXIT_FAILURE);
    }
  umask(0);

  if((chdir("/")) < 0)
    {
      WriteLog("Can't change directory");
      exit(EXIT_FAILURE);
    }

  fclose(stderr);
  fclose(stdin);
  fclose(stdout);


  if((chdir(cfg.rootdir)))
    {
      WriteLogPError(cfg.rootdir);
      WriteLog("Exit");
      exit(EXIT_FAILURE);
    }
  sigemptyset(&set);
  // signals
  sigaddset(&set, SIGPIPE );
  //sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
  sigprocmask(SIG_BLOCK,&set, NULL);

  //TODO signal handler(SIGPIPE, SIGUSR1 etc)
  create_worker();

}
