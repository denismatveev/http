#include "parse_config.h"
#include "http.h"
#include "string.h"
#include "stdlib.h"

int parse_cfg(config *cfg, const char* fname)
{
  FILE *f;
  int r=0;
  char str[256];
  int err;
  char del='=';
  char end=';';
  int port;
  char *strparam;


  WriteLog("Parsing config file %s", fname);
  fprintf(stdout,"Parsing config file\n");

  if((f=fopen(fname,"r")) == NULL)
  {
    err=errno;
    WriteLogPError(fname);

    return -1;
  }


  while((fgets(str, 256, f)) != NULL)
  {
    if(!(strncmp(str,"port",4)))
    {
      if((strparam=parse_str(str,del, end)) == NULL)
      {
        WriteLog("Invalid config near %s",str);
        return -1;
      }
      port=atoi(strparam);
      cfg->listen.sin_port=htons(port);
      WriteLog("Port number = %i\n",port);
    }
    else
      if(!(strncmp(str,"listen", 6)))
      {
        if((strparam=parse_str(str,del,end)) == NULL)
        {
          WriteLog("Invalid config near %s",str);
          return -1;
        }

        if((r=inet_pton(AF_INET,strparam,&cfg->listen.sin_addr.s_addr)) == 1)
        {
          cfg->listen.sin_family=AF_INET;
          WriteLog("Listen to %s", strparam);
        }
        else
        {
          WriteLog("Invalid IP address");
          return -1;
        }
      }
      else
        if(!(strncmp(str,"workers", 7)))
        {
          if((strparam=parse_str(str,del,end)) == NULL)
          {
            WriteLog("Invalid config near %s",str);
            return -1;
          }
          cfg->workers=atoi(strparam);
          WriteLog("Number of workers = %i",cfg->workers);
        }
        else
          if(!(strncmp(str,"rootdir",7)))
          {
            if((strparam=parse_str(str,del,end)) == NULL)
            {
              WriteLog("Invalid config near %s",str);
              return -1;
            }
            strncpy(cfg->rootdir,strparam,255);
            WriteLog("Setting rootdir = %s",cfg->rootdir);
          }
  }

}

char* parse_str(char* wholestr, char delim, char ending)
{
  char *r,*e, *strend;

  if((strend=strchr(wholestr, '#')) != NULL)
    *strend=0;
  if((r=strchr(wholestr,delim)) != NULL)
  {
    if((e=strchr(++r, ending)) == NULL)
      return NULL;
    *e=0;

    return r;
  }
  else
    return NULL;

}
