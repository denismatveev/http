#ifndef PARSE_CONFIG_H
#define PARSE_CONFIG_H
#include<stdio.h>
#include <arpa/inet.h>

 typedef struct _config
 {
   char rootdir[1024];
   struct sockaddr_in listen;
   int workers;
 } config_t;
void default_cfg(config_t*);
int parse_cfg(config_t *cfg, const char* fname);
char* parse_str(char* wholestr, char delim, char ending);
extern config_t cfg;

int check_config();

#endif //PARSE_CONFIG_H
