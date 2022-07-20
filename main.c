#include "http.h"
#include "config.h"
#include "worker.h"
#include "common.h"
#include <pthread.h>
#include <signal.h>

static char *cfgFile = NULL;
config_t* cfg;
int main(int argc, char** argv)
{

    pid_t pid;
    int c;
    sigset_t set;
    char* help_message="Usage: %s -c <config> - start web server\n"
                       "             -h\t\t - print help message and exit\n";


    while ((c = getopt (argc, argv, ":hc:")) != -1)
    {
        switch (c)
        {
        case 'c':
            cfgFile = optarg;
            break;
        case 'h':
            fprintf(stderr,help_message,argv[0]);
            exit(EXIT_SUCCESS);
        case '?':
            fprintf(stderr, "Unknown option -%c\n", optopt);
            fprintf(stderr,help_message,argv[0]);
            exit(EXIT_FAILURE);
        case ':':
            fprintf(stderr, "Option -%c requires an argument\n", optopt);
            fprintf(stderr,help_message,argv[0]);
            exit(EXIT_FAILURE);
        default:
            fprintf(stderr,help_message,argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if(cfgFile == NULL)
    {
        fprintf(stderr,help_message,argv[0]);
        WriteLog("No cfg file provided");
        exit(EXIT_FAILURE);

    }

    if((create_cfg_from_file(&cfg, cfgFile)))
    {
        printf("Something went wrong. Check logs\n");
        exit(1);
    }
    if((check_cfg(cfg) < 0))
    {
        printf("Wrong config. Check logs\n");
        exit(1);
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
        //exit(EXIT_SUCCESS);
    }
    /* the following code is executed in a child process */
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


    sigemptyset(&set);
    // signals
    sigaddset(&set, SIGPIPE );
    //sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
    sigprocmask(SIG_BLOCK,&set, NULL);

    //TODO signal handler(SIGPIPE, SIGUSR1 etc)
    run_server();

    //    destroy_config(cfg);
}
