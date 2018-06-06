#ifndef __PARSE_CONFIG_H__
#define __PARSE_CONFIG_H__
#include<stdio.h>
#include <arpa/inet.h>


 typedef struct _config
 {
   char rootdir[1024];
   struct sockaddr_in listen;
   char workers;
 } config_t;
void default_cfg(config_t*);
int parse_cfg(config_t *cfg, const char* fname);
char* parse_str(char* wholestr, char delim, char ending);
#endif //__PARSE_CONFIG_H__
