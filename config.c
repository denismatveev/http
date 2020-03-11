#include<string.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include"config.h"
#include"queue.h"
#include"common.h"

/* config is implelented as RB tree where each node contains a section. Section in its turn contains associative array(key=value) for all domains.
 * Associative domain implelemted as hash map with open addressing.
 * RB tree implemented with the following properties:
 * - The root is always black
 * - end leaves are black
 * - Child of red is black
 * - Black depth is constant
 * For destroying the tree queue is used(as linked list) to store children of visited nodes and then process them
 */

// init_section() is initializing section
// section is assoc array to store virtual host settings like index, root directory etc
// section is RB tree node as well
/* Here is an an example of server config */
/*
 * <General>
port = 8080;
listen =127.0.0.1;
workers =5;#test
timeout = 300;
</General>
<Host>
name=denis.ilmen-tau.ru
rootdir=/var/www/denis/
indexfile=index.html
</Host>
<Host>
name=nastya.ilmen-tau.ru
rootdir=/var/www/nastya
indexfile=index.html
</Host>
* */

char *hosts_reserved_names[] = {
    "rootdir",
    "indexfile",
    "servername",
    NULL
};
char *general_reserved_names[] = {
    "port",
    "listen",
    "workers",
    "timeout",
    NULL
};
int check_general_reserved_key(char* key)
{
    int i=0;

    while(general_reserved_names[i] != NULL)
    {
        if((strncmp(key,general_reserved_names[i],10)) == 0)
            return 0;
        i++;
    }
    return INVALID_GENERAL_KEY;

}
int check_vhost_reserved_key(char* key)
{
    int i=0;

    while(hosts_reserved_names[i] != NULL)
    {
        if((strncmp(key,hosts_reserved_names[i],10)) == 0)
            return 0;
        i++;
    }
    return INVALID_HOSTS_KEY;
}
section_t init_section(section_type_t t)
{
    section_t s;
    assoc_t *arr;

    arr=init_assoc_array();

    if((s=(section_t)malloc(sizeof (__section_t))) == NULL)
        return NULL;
    s->type = t;
    s->Set=arr;

    return s;

}
void destroy_section(section_t t)
{
    if(t == NULL)
        return;
    destroy_assoc_array(t->Set);
    free(t);

    return;
}

// fill_section() function is adding key-values into assoc array
int fill_section(section_t s, const char* key, const char* value)
{
    int ret;

    if(key == NULL || value == NULL || s == NULL)
        return 1;

    if(strlen(key) == 0 || strlen(value) == 0)
        return 1;

    if((ret=add_elem(s->Set, key, value) != 0))
        return 1;

    return 0;
}

int parse_str(char* wholestr, char* key, char* value, char comment, char delim, char ending)
{
    char *r, *strend;
    char delims[2];
    delims[0]=delim;
    delims[1]=0;
    char wholestr_cpy[256];
    char* saveptr;
    strncpy(wholestr_cpy, wholestr, 256);

    if((strend=strchr(wholestr_cpy, comment)) != NULL)
        *strend=0;
    if((strend=strchr(wholestr_cpy, ending)) != NULL)
        *strend=0;

    if((r=strtok_r(wholestr_cpy, delims,&saveptr)) != NULL)
    {
        strncpy(key, r, 256);

        if((r=strtok_r(NULL,delims,&saveptr)))
        {
            strncpy(value, r, 256);
            return 0;
        }
    }
    return 1;
}

config_t* init_config()
{
    config_t* cfg;

    if((cfg=(config_t*)malloc(sizeof (config_t))) == NULL)
    {
        WriteLogPError("Allocating error while config is being created");
        return NULL;
    }

    if((cfg->general=init_section(General)) == NULL)
        return NULL;
    if((cfg->default_vhost=init_section(Default)) == NULL)
        return NULL;
    cfg->hosts=NULL;
    return cfg;
}

void destroy_config(config_t* cfg)
{
    if(cfg == NULL)
        return;

    destroy_section(cfg->general);
    destroy_section(cfg->default_vhost);
    destroy_rbtree(cfg->hosts);

    free(cfg);

    return;
}

int create_config(config_t *cfg, const char* fname)
{
    node_t* n;
    FILE *f;
    long fsize;
    long pos=0;
    section_t s;

    WriteLog("Parsing config file %s", fname);

    if((f=fopen(fname,"r")) == NULL)
    {
        WriteLogPError(fname);
        return 2;
    }

    if(fseek(f, 0L, SEEK_SET) != 0)
    {
        WriteLogPError("Seeking the beginning of config file");
        return -1;

    }
    if(fseek(f,0L, SEEK_END) != 0)
    {
        WriteLogPError("Setting the end of config file");
        return -1;
    }
    if((fsize=ftell(f)) == -1)
    {
        WriteLogPError("Telling file position");
        return -1;
    }
    if(fseek(f, 0L, SEEK_SET) != 0)
    {
        WriteLogPError("Config file");
        return -1;
    }


    cfg=init_config();
    if(cfg == NULL)
    {
        WriteLog("Cannot create config");
        return 1;
    }


    if((cfg->hosts=init_rbtree(n)) == NULL)
    {
        WriteLogPError("Initializing config");
        return 1;
    }

    while(pos < fsize)
    {
        pos=get_section_from_cfg(s, f);
        if(s->type == General)
        {
            cfg->general=s;

        }
        else if(s->type == Default)
        {
            cfg->general=s;
        }
        else if(s->type == Host)
        {
            n=init_node_s(s);
            cfg->hosts=init_rbtree(n);
        }
        else
        {
            WriteLog("Wrong config");
            return 1;
        }

    }

    fclose(f);
    return 0;
}

long get_section_from_cfg(section_t section, FILE* f)
{
    char str[256];
    char del='=';
    char end=';';
    char key[256];
    char val[256];
    char comment_sign='#';
    section_type_t st;
    uint16_t port;
    char *general_section_begin="<General>";
    char *general_section_end="</General>";
    char *virtual_server_section_begin="<Host>";
    char *virtual_server_section_end="</Host>";
    char *default_server_section_begin="<Default>";
    char *default_server_section_end="</Default>";
    unsigned long general_section_begin_len=strlen(general_section_begin);
    unsigned long general_section_end_len=strlen(general_section_end);
    unsigned long virtual_server_section_begin_len=strlen(virtual_server_section_begin);
    unsigned long virtual_server_section_end_len=strlen(virtual_server_section_end);
    unsigned long default_server_section_begin_len=strlen(default_server_section_begin);
    unsigned long default_server_section_end_len=strlen(default_server_section_end);
    section=init_section(st);
    while((fgets(str, 256, f)) != NULL)
    {
        if(!(strncmp(str,general_section_begin,general_section_begin_len)))
        {
            while((fgets(str, 256, f)) != NULL)
            {
                if(strncmp(str, general_section_end, general_section_end_len))
                {
                    if((parse_str(str, key, val, comment_sign, del, end)) != 0)
                    {
                        WriteLog("Invalid config near %s",str);
                        return -1;
                    } else if(check_general_reserved_key(key))
                    {
                        WriteLog("Unacceptable %s in general section",key);
                        return -1;
                    }
                    fill_section(section, key, val);
                    section->type=General;

                }
                return ftell(f);
            }
        }
        else if(!(strncmp(str,virtual_server_section_begin, virtual_server_section_begin_len)))
        {
            while((fgets(str, 256, f)) != NULL)
            {
                if(strncmp(str, virtual_server_section_end, virtual_server_section_end_len))
                {
                    if((parse_str(str,key, val, comment_sign, del, end)) != 0)
                    {
                        WriteLog("Invalid config near %s",str);
                        return -1;
                    } else if(check_vhost_reserved_key(key))
                    {
                        WriteLog("Unacceptable %s in virtual host section",key);
                        return -1;
                    }
                    fill_section(section, key, val);
                    section->type=Host;
                }
                return ftell(f);
            }

        }else if(!(strncmp(str,default_server_section_begin, default_server_section_begin_len)))
        {
            while((fgets(str, 256, f)) != NULL)
            {
                if(strncmp(str, default_server_section_end, default_server_section_end_len))
                {
                    if((parse_str(str,key, val, comment_sign, del, end)) != 0)
                    {
                        WriteLog("Invalid config near %s",str);
                        return -1;
                    } else if(check_vhost_reserved_key(key))
                    {
                        WriteLog("Unacceptable %s in virtual host section(default host)",key);
                        return -1;
                    }
                    fill_section(section, key, val);
                    section->type=Default;
                }
                return ftell(f);
            }
        }
    }


    destroy_section(section);
    return -1;

}

section_t search_for_host(const config_t *c, const char* host)
{
    node_t* n;
    n=search_host(c->hosts, host);

    return n ? n->section : NULL;
}

// TODO check_config(config_t* cfg);
