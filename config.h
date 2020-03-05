#ifndef CONFIG_TREE_H
#define CONFIG_TREE_H
#include<stdio.h>
#include"associative.h"

typedef enum section_type
{
    General=0,// General section contains info such as port, timeout, workers,listen interface etc
    Default=1,// this config is used if any of 'Host' config is not suitable
    Host=2 // Host config describes virtual host
}section_type_t;

typedef struct __section
{
    section_type_t type;
    assoc_t* Set;
}__section_t;

typedef __section_t* section_t;

section_t init_section(section_type_t t);
int fill_section(section_t s, const char* key, const char* value);
void destroy_section(section_t t);

#endif //CONFIG_TREE_H
