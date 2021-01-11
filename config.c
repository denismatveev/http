#include<string.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include"config.h"
#include"queue.h"
#include"common.h"
#define comment_sign '#'
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
/* Here is an example of server config */
/*
# config
<General>
port = 8080;
#rootdir =/var/www/;
listen =127.0.0.1;
workers =5;#test
</General>
<Host>
servername=site1.example.com;
rootdir=/var/www/site1;
indexfile=index.html;
</Host>
<Host>
servername=site2.example.com;
rootdir=/var/www/site2;
indexfile=index.html;
</Host>
<Default>
servername=site3.example.com;
rootdir=/var/www/site3;
indexfile=index.htm;
</Default>
# end of config
*/

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
/* Important! Take into account the order of section names is considered ! */
char* opening_section_names[] = {
    "<General>",
    "<Default>",
    "<Host>",
    NULL
};
char* closing_section_names[] = {
    "</General>",
    "</Default>",
    "</Host>",
    NULL
};
int check_closing_section_name(char* sect)
{
    int i=0;

    while(closing_section_names[i] != NULL)
    {
        if((strncmp(sect,closing_section_names[i],11)) == 0)
            return 0;
        i++;
    }
    return INVALID_SECTION_NAME;
}
int check_opening_section_name(char* sect)
{
    int i=0;

    while(opening_section_names[i] != NULL)
    {
        if((strncmp(sect,opening_section_names[i],11)) == 0)
            return 0;
        i++;
    }
    return INVALID_SECTION_NAME;
}
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
    if(check_if_str_commented_or_blank(wholestr, comment, 256))
        return 2;
    strncpy(wholestr_cpy, wholestr, 256);

    if((strend=strchr(wholestr_cpy, comment)) != NULL)
        *strend=0;
    if((strend=strchr(wholestr_cpy, ending)) != NULL)
        *strend=0;
    else
        return 3;


    if((r=strtok_r(wholestr_cpy, delims,&saveptr)) != NULL)
    {
        strncpy(key, r, 256);

        if((r=strtok_r(NULL,delims,&saveptr)))
        {
            strncpy(value, r, 256);
            return 0;
        }
        return 1;
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

    if((cfg->general=init_section(Section_General)) == NULL)
        return NULL;
    if((cfg->default_vhost=init_section(Section_Default)) == NULL)
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

int create_config(config_t **cfg, const char* fname)
{
    node_t* n = NULL;
    FILE *f;
    long fsize;
    long pos=0;
    section_t s = NULL;
    char sitename[256]={0};

    WriteLog("Parsing config file %s", fname);

    if((f=fopen(fname,"r")) == NULL)
    {
        WriteLogPError(fname);
        return 2;
    }

    if(fseek(f, 0L, SEEK_SET) != 0)
    {
        WriteLogPError("Searching for the beginning of config file");
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


    *cfg=init_config();
    if(cfg == NULL)
    {
        WriteLog("Cannot create config");
        return 1;
    }

    while(pos < fsize)
    {
        if((pos=get_section_from_cfg(&s, sitename, f)) == -1)
        {
            fclose(f);
            return -1;
        }
        if(s->type == Section_General)
        {
            (*cfg)->general=s;
        }
        else if(s->type == Section_Default)
        {
            (*cfg)->default_vhost=s;
        }
        else if(s->type == Section_Host)
        {
            n=init_node_s(s);
            strncpy(n->hostname,sitename,256);
            if((*cfg)->hosts == NULL)
                (*cfg)->hosts=init_rbtree(n);
            else
                insert_in_rbtree((*cfg)->hosts, n);
        }
    }

    fclose(f);
    WriteLog("Parsing config: OK");
    return 0;
}

int check_balanced_closing_section_name(char* str, section_type_t st)
{
    size_t len=strlen(str);
    if(len == 0)
        return 1;

    if((strncmp(str, closing_section_names[st], len)) == 0)
        return 0;
    if(!(check_closing_section_name(str)))
    {
        WriteLog("Found wrong closing section '%s'", str);
        return -1;
    }
    if(!(check_opening_section_name(str)))
    {
        WriteLog("Found wrong opening section '%s'", str);
        return -1;
    }
    return 1;
}

int config_read_section(FILE* f, section_t* section, char* sitename, int *section_counter, section_type_t s)
{
    char str[256]={0};
    char del='=';
    char end=';';
    char key[256];
    char val[256];
    int r, rc;
    size_t len;
    switch (s)
    {
    case 0:
        /* General config */
    {

        len=strlen(closing_section_names[Section_General]);
        *section=init_section(Section_General);
        while((fgets(str, 256, f)) != NULL)
        {
            replace_trailing(str);
            removeSpaces(str);
            if((rc=check_balanced_closing_section_name(str, Section_General)) == 0)
            {
                (*section_counter)--;
                return 0;

            }
            else if (rc == -1)
                return -1;

            if((r=parse_str(str, key, val, comment_sign, del, end)) == 1)
            {
                WriteLog("Invalid config near '%s'", str);
                return -1;
            }
            else if(r == 2)/* empty string */
            {
                continue;
            }
            else if (r == 3)
            {
                WriteLog("No termination ';' in general section near '%s'", str);
                return -1;
            }
            else if(check_general_reserved_key(key))
            {
                WriteLog("Invalid param '%s' in general section", key);
                return -1;
            }

            fill_section(*section, key, val);

        }
        if((*section_counter != 0))
        {
            WriteLog("Wrong config in section %s", opening_section_names[Section_General]);
            return -1;
        }
    }

        break;


    case 1:
        /* Default config */
    {
        len=strlen(closing_section_names[Section_Default]);
        *section=init_section(Section_Default);
        while((fgets(str, 256, f)) != NULL)
        {
            removeSpaces(str);
            replace_trailing(str);
            if((rc=check_balanced_closing_section_name(str, Section_Default)) == 0)
            {
                (*section_counter)--;
                return 0;

            }
            else if (rc == -1)
                return -1;

            if((r=parse_str(str, key, val, comment_sign, del, end)) == 1)
            {
                WriteLog("Invalid config near '%s'", str);
                return -1;
            }
            else if(r == 2)/* empty string */
            {
                continue;
            }
            else if (r == 3)
            {
                WriteLog("No termination ';' in default host section near '%s'", str);
                return -1;
            }
            else if(check_vhost_reserved_key(key))
            {
                WriteLog("Invalid param '%s' in default host section", key);
                return -1;
            }
            fill_section(*section, key, val);
        }
        if((*section_counter != 0))
        {
            WriteLog("Wrong config in section %s", opening_section_names[Section_Default]);
            return -1;
        }

    }

        break;
    case 2:
        /* Host config */
    {

        len=strlen(closing_section_names[Section_Host]);
        *section=init_section(Section_Host);
        while((fgets(str, 256, f)) != NULL)
        {
            removeSpaces(str);
            replace_trailing(str);
            if((rc=check_balanced_closing_section_name(str, Section_Host)) == 0)
            {
                (*section_counter)--;
                return 0;

            }
            else if (rc == -1)
                return -1;

            if((r=parse_str(str,key, val, comment_sign, del, end)) == 1)
            {
                WriteLog("Invalid config near '%s'", str);
                return -1;
            }
            else if(r == 2)/* empty string */
            {
                continue;
            }
            else if (r == 3)
            {
                WriteLog("No termination ';' in virtual host section near '%s'", str);
                return -1;
            }
            else if(check_vhost_reserved_key(key))
            {
                WriteLog("Invalid param '%s' in virtual host section", key);
                return -1;
            }

            if((fill_section(*section, key, val) == 0))
                if((strncmp(key, hosts_reserved_names[2], 256)) == 0)
                    strncpy(sitename, val, 256);
        }
        if((*section_counter != 0))
        {
            WriteLog("Wrong config in section %s", opening_section_names[Section_Host]);
            return -1;
        }

    }
        break;
    }

    return 0;

}

long get_section_from_cfg(section_t* section, char* sitename, FILE* f)
{
    char str[256]={0};
    int section_counter=0;
    int r=0;

    //uint16_t port;

    while((fgets(str, 256, f)) != NULL)
    {
        replace_trailing(str);
        if((check_if_str_commented_or_blank(str, comment_sign, 256)) == 1)
            continue;
        if(!(strncmp(str,opening_section_names[Section_General],strlen(opening_section_names[Section_General]))))
        {
            section_counter++;
            if((r=config_read_section(f, section, sitename, &section_counter, Section_General)) == 0)
                return ftell(f);
            else
                return -1;
        }
        else if(!(strncmp(str,opening_section_names[Section_Default], strlen(opening_section_names[Section_Default]))))
        {
            section_counter++;
            if((r=config_read_section(f, section, sitename, &section_counter, Section_Default)) == 0)
                return ftell(f);
            else
                return -1;
        }
        else if(!(strncmp(str,opening_section_names[Section_Host], strlen(opening_section_names[Section_Host]))))
        {
            section_counter++;
            if((r=config_read_section(f, section, sitename, &section_counter, Section_Host)) == 0)
                return ftell(f);
            else
                return -1;
        }
        else
        {
            WriteLog("No correct opening section near '%s'", str);
            return -1;

        }
    }
    if(section_counter != 0)
    {
        WriteLog("Wrong config near '%s'", str);
        return -1;
    }
    return ftell(f);
}

section_t search_for_host(const config_t *c, const char* host)
{
    node_t* n;
    n=search_host(c->hosts, host);

    return n ? n->section : NULL;
}

// TODO check_config(config_t* cfg);
//void print_cfg(config_t* cfg);
