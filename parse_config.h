#ifndef __PARSE_CONFIG
#define __PARSE_CONFIG
#include<stdio.h>
#include <arpa/inet.h>

 typedef struct _config
 {
   char rootdir[1024];
   struct sockaddr_in listen;
   char workers;
 } config;

int parse_cfg(config *cfg, const char* fname);
char* parse_str(char* wholestr, char delim, char ending);
#endif //__PARSE_CONFIG
