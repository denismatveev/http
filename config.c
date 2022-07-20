#include<string.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include"config.h"
#include"queue.h"
#include"common.h"
#define comment_sign '#'
/* config is implemented as RB tree where each node contains a section. Section in its turn contains associative array(key=value) for all domains.
 * Associative domain implelemted as hash map with open addressing.
 * RB tree implemented with the following properties:
 * - The root is always black
 * - end leaves are black
 * - Child of red is black
 * - Black depth is constant
 * For destroying the tree a queue is used(as a linked list) to store children of visited nodes and then processing them
 */

// init_section() is initializing section
// section is assoc array to store virtual host settings like index, root directory etc
// section is RB tree node as well
// General and Default sections are not nodes of red-black tree
// The logic is the following: search for hostname in rbtree, if nothing found, use default section
// General section is used only at the launch moment


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
indexfile=index.html;
</Default>
# end of config
*/

// TODO implement aliases
// TODO duplicated indexfile is also can be considered as additional file for searching, let's say index.html and index.php

static unsigned int str_number=0;

char *hosts_reserved_names[] =
{
    #define XX(num, name, hosts_reserved_names) #hosts_reserved_names,
    HOSTS_RESERVED_NAMES(XX)
    #undef XX
    NULL
};
char *general_reserved_names[] =
{
    #define XX(num, name, general_reserved_names) #general_reserved_names,
    GENERAL_RESERVED_NAMES(XX)
    #undef XX
    NULL
};
// TODO replace explicit defining array of chars by Macro X and put all definitions into h file
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
    if((check_if_cfg_param_exists_in_section(s, key)))
        return 2;

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

config_t* init_cfg()
{
    config_t* cfg;

    if((cfg=(config_t*)malloc(sizeof (config_t))) == NULL)
    {
        WriteLogPError("Allocating error while config is being created");
        return NULL;
    }
    cfg->general = NULL;
    cfg->default_vhost = NULL;
    cfg->hosts=NULL;
    return cfg;
}

void destroy_cfg(config_t* cfg)
{
    if(cfg == NULL)
        return;

    destroy_section(cfg->general);
    if(cfg->default_vhost != NULL)
        destroy_section(cfg->default_vhost);
    if(cfg->hosts != NULL)
        destroy_rbtree(cfg->hosts);

    free(cfg);

    return;
}

int create_cfg_from_file(config_t **cfg, const char* fname)
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
        goto on_file_error;

    }
    if(fseek(f,0L, SEEK_END) != 0)
    {
        WriteLogPError("Setting the end of config file");
        goto on_file_error;
    }
    if((fsize=ftell(f)) == -1)
    {
        WriteLogPError("Telling file position");
        goto on_file_error;
    }
    if(fseek(f, 0L, SEEK_SET) != 0)
    {
        WriteLogPError("Config file");
        goto on_file_error;
    }


    *cfg=init_cfg();
    if(cfg == NULL)
    {
        WriteLog("Cannot create config");
        fclose(f);
        return 1;
    }

    while(pos < fsize)
    {
        s=NULL;
        n=NULL;
        memset(sitename, 0,sizeof (sitename));
        if((pos=get_section_data_from_config_file(&s, sitename, f)) == -1)
        {
            fclose(f);
            return -1;
        }
        if(s != NULL)
        {
            switch(s->type)
            {
            case Section_General:
                (*cfg)->general=s;
                break;
            case Section_Default:
                (*cfg)->default_vhost=s;
                break;
            case Section_Host:
            {
                n=init_node_s(s);
                strncpy(n->hostname,sitename,256);
                if((*cfg)->hosts == NULL)
                    (*cfg)->hosts=init_rbtree(n);
                else
                    if((insert_in_rbtree((*cfg)->hosts, n)) == NULL)
                    {
                        WriteLog("Duplicated virtual host %s in %s section in line %i", n->hostname, opening_section_names[Section_Host], str_number);
                        destroy_node(n);
                        goto on_file_error;
                    }
                break;
            }
            }
        }
    }

    fclose(f);
    WriteLog("Parsing config: OK");
    return 0;
on_file_error:
    fclose(f);
    return -1;
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
        //   WriteLog("Found wrong closing section '%s'", str);
        return -1;
    }
    if(!(check_opening_section_name(str)))
    {
        //    WriteLog("Found wrong opening section '%s'", str);
        return -2;
    }
    return 1;
}

int read_config_section_from_file(FILE* f, section_t* section, char* sitename, int *section_counter, section_type_t s)
{
    char str[256]={0};
    char orig_str[256]={0};
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
            str_number++;
            replace_trailing(str);
            strncpy(orig_str, str, 256);
            removeSpaces(str);
            if((rc=check_balanced_closing_section_name(orig_str, Section_General)) == 0)
            {
                (*section_counter)--;
                return 0;

            }
            else if (rc == -1)
            {
                WriteLog("Found wrong closing section '%s' in line %i", orig_str, str_number);
                return -1;

            }
            else if( rc == -2)
            {
                WriteLog("Found wrong opening section '%s' in line %i", orig_str, str_number);
                return -1;
            }

            if((rc=check_balanced_closing_section_name(str, Section_General)) == 0)
            {
                WriteLog("Found spaces in closing section in %s: %s in line %i",closing_section_names[Section_General], orig_str, str_number);
                return -1;
            }
            else if(rc == -1)
            {
                WriteLog("Wrong closing section '%s' has spaces in line %i",orig_str, str_number);

                return -1;

            }
            else if( rc == -2)
            {
                WriteLog("Wrong opening section '%s' has spaces in line %i", orig_str, str_number);
                return -1;
            }

            if((r=parse_str(str, key, val, comment_sign, del, end)) == 1)
            {
                WriteLog("Invalid config near '%s' in line %i", str, str_number);
                return -1;
            }
            else if(r == 2)/* empty string */
            {
                continue;
            }
            else if (r == 3)
            {
                WriteLog("No termination ';' in general section near '%s' in line %i", str, str_number);
                return -1;
            }
            else if(check_general_reserved_key(key))
            {
                WriteLog("Invalid param '%s' in %s section in line %i", key, opening_section_names[Section_General], str_number);
                return -1;
            }

            if((fill_section(*section, key, val)) == 2)
            {
                WriteLog("Found duplicated parameter '%s' in line %i", key, str_number);
                return -1;
            }

        }
        if((*section_counter != 0))
        {
            WriteLog("Wrong config in section %s in line %i", opening_section_names[Section_General], str_number);
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

            str_number++;
            replace_trailing(str);
            strncpy(orig_str, str, 256);
            removeSpaces(str);
            if((rc=check_balanced_closing_section_name(orig_str, Section_Default)) == 0)
            {
                (*section_counter)--;
                return 0;

            }
            else if (rc == -1)
            {
                WriteLog("Found wrong closing section '%s' in line %i", orig_str, str_number);
                return -1;

            }
            else if( rc == -2)
            {
                WriteLog("Found wrong opening section '%s' in line %i", orig_str, str_number);
                return -1;
            }
            removeSpaces(str);
            if((rc=check_balanced_closing_section_name(str, Section_Default)) == 0)
            {
                WriteLog("Found spaces in closing section in %s: %s in line %i",closing_section_names[Section_Default], orig_str, str_number);
                return -1;
            }
            else if(rc == -1)
            {
                WriteLog("Wrong closing section '%s' has spaces in line %i",orig_str, str_number);

                return -1;

            }
            else if( rc == -2)
            {
                WriteLog("Wrong opening section '%s' has spaces in line %i", orig_str, str_number);
                return -1;
            }
            if((r=parse_str(str, key, val, comment_sign, del, end)) == 1)
            {
                WriteLog("Invalid config near '%s' in line %i", str, str_number);
                return -1;
            }
            else if(r == 2)/* empty string */
            {
                continue;
            }
            else if (r == 3)
            {
                WriteLog("No termination ';' in default host section near '%s' in line %i", str, str_number);
                return -1;
            }
            else if(check_vhost_reserved_key(key))
            {
                WriteLog("Invalid param '%s' in default host section in line %i", key, str_number);
                return -1;
            }

            if((fill_section(*section, key, val)) == 2)
            {
                WriteLog("Found duplicated parameter '%s' in line %i", key, str_number);
                return -1;
            }
        }
        if((*section_counter != 0))
        {
            WriteLog("Wrong config in section %s in line %i", opening_section_names[Section_Default], str_number);
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
            str_number++;
            replace_trailing(str);
            strncpy(orig_str, str, 256);
            removeSpaces(str);
            if((rc=check_balanced_closing_section_name(orig_str, Section_Host)) == 0)
            {
                (*section_counter)--;
                return 0;

            }
            else if (rc == -1)
            {
                WriteLog("Found wrong closing section '%s' in line %i", orig_str, str_number);
                return -1;

            }
            else if( rc == -2)
            {
                WriteLog("Found wrong opening section '%s' in line %i", orig_str, str_number);
                return -1;
            }
            removeSpaces(str);
            if((rc=check_balanced_closing_section_name(str, Section_Host)) == 0)
            {
                WriteLog("Found spaces in closing section in %s: %s in line %i",closing_section_names[Section_Host], orig_str, str_number);
                return -1;
            }
            else if(rc == -1)
            {
                WriteLog("Wrong closing section '%s' has spaces in line %i",orig_str, str_number);

                return -1;

            }
            else if( rc == -2)
            {
                WriteLog("Wrong opening section '%s' has spaces in line %i", orig_str, str_number);
                return -1;
            }
            if((r=parse_str(str,key, val, comment_sign, del, end)) == 1)
            {
                WriteLog("Invalid config near '%s' in line %i", str, str_number);
                return -1;
            }
            else if(r == 2)/* empty string */
            {
                continue;
            }
            else if (r == 3)
            {
                WriteLog("No termination ';' in virtual host section near '%s' in line %i", str, str_number);
                return -1;
            }
            else if(check_vhost_reserved_key(key))
            {
                WriteLog("Invalid param '%s' in virtual host section in line %i", key, str_number);
                return -1;
            }

            if((rc=fill_section(*section, key, val)) == 0)
            {
                if((strncmp(key, hosts_reserved_names[2], 256)) == 0)
                    strncpy(sitename, val, 256);
            }
            else if (rc == 2)
            {
                WriteLog("Found duplicated parameter '%s' in line %i", key, str_number);
                return -1;
            }
        }
        if((*section_counter != 0))
        {
            WriteLog("Wrong config in section %s in line %i", opening_section_names[Section_Host], str_number);
            return -1;
        }

    }
        break;
    }

    return 0;

}

long get_section_data_from_config_file(section_t* section, char* sitename, FILE* f)
{
    char str[256]={0};
    int section_counter=0;
    int r=0;

    //uint16_t port;

    while((fgets(str, 256, f)) != NULL)
    {
        str_number++;
        replace_trailing(str);
        if((check_if_str_commented_or_blank(str, comment_sign, 256)) == 1)
            continue;
        if(!(strncmp(str,opening_section_names[Section_General],strlen(opening_section_names[Section_General]))))
        {
            section_counter++;
            if((r=read_config_section_from_file(f, section, sitename, &section_counter, Section_General)) == 0)
                return ftell(f);
            else
                return -1;
        }
        else if(!(strncmp(str,opening_section_names[Section_Default], strlen(opening_section_names[Section_Default]))))
        {
            section_counter++;
            if((r=read_config_section_from_file(f, section, sitename, &section_counter, Section_Default)) == 0)
                return ftell(f);
            else
                return -1;
        }
        else if(!(strncmp(str,opening_section_names[Section_Host], strlen(opening_section_names[Section_Host]))))
        {
            section_counter++;
            if((r=read_config_section_from_file(f, section, sitename, &section_counter, Section_Host)) == 0)
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

section_t search_for_host_in_cfg(const config_t *c, const char* host)
{
    node_t* n = NULL;
    if(c->hosts != NULL)
        n=search_host(c->hosts, host);

    return n ? n->section : c->default_vhost;
}

int check_cfg(config_t* cfg)
{
    // Funtion checks if General section is empty
    // Function checks if Default is emplty
    // check <General> section
    char* site;
    char* param=NULL;
    params_t args;
    args.param=NULL;
    args.sitename=NULL;

    if(cfg->general == NULL)
    {
        WriteLog("No mandatory section %s found in config",opening_section_names[Section_General]);
        return -1;
    }

    // check <Default> and <Hosts> section
    if(cfg->default_vhost == NULL && cfg->hosts == NULL)
    {
        WriteLog("Either %s or %s section must be specified\n",opening_section_names[Section_Default], opening_section_names[Section_Host]);
        return -1;
    }

    if((check_section(cfg->general, &param)) != 0 )
    {
        WriteLog("Config in section %s does not have necessary parameter '%s'",opening_section_names[cfg->general->type], param );
        return -1;
    }
    if(cfg->default_vhost != NULL)
    {

        if((check_section(cfg->default_vhost, &param)) != 0 )
        {
            WriteLog("Config in section %s does not have necessary parameter '%s'",opening_section_names[cfg->default_vhost->type], param );
            return -1;
        }
    }
    else if(cfg->hosts != NULL)
    {

        level_order_traverseRBTree(cfg->hosts, (void*)check_section_in_RBtree, &args);
        if(args.param != NULL)
        {
            if((strncmp(args.param, hosts_reserved_names[2], 11)))
            {
                WriteLog("Config in section %s does not have necessary parameter '%s' in '%s' virtual host",opening_section_names[2], args.param, args.sitename );
                return -1;
            }
            else
            {
                WriteLog("Config in section %s does not have necessary parameter '%s'",opening_section_names[2], args.param );
                return -1;
            }
        }
    }

    // Before using the function below you must ensure cfg configured properly
    if((site=check_cfg_for_duplicated_sitename(cfg)) != NULL)
    {
        WriteLog("Duplicated virtual host %s in %s section", site, opening_section_names[Section_Default]);
        return -1;
    }
    // Before using the function below you must ensure cfg configured properly
    if((load_cfg(cfg)) == -1)
        return -1;
    return 0;

}
int check_section_in_RBtree(node_t* n, void* args)
{

    if((check_section(n->section,&((params_t*)args)->param)) == -1)
    {
        ((params_t*)args)->sitename=n->hostname;
        return -1;

    }
    return 0;
}
int check_section(section_t section, char** absent_param)
{
    if(section->type == Section_General)
    {
        for(int i=0;general_reserved_names[i] != NULL;i++)
            if(!(check_if_cfg_param_exists_in_section(section, general_reserved_names[i])))
            {
                *absent_param=general_reserved_names[i];
                return -1;
            }
    }
    else
        for(int i=0;hosts_reserved_names[i] != NULL;i++)
            if(!(check_if_cfg_param_exists_in_section(section, hosts_reserved_names[i])))
            {
                *absent_param=hosts_reserved_names[i];
                return -1;
            }
    return 0;
}
char* check_cfg_for_duplicated_sitename(config_t* cfg)
{
    // function matches only if a virtual host in <Default> section is duplicated in <Host> section
    char* default_name;
    if(cfg->default_vhost != NULL)
    {
        default_name=get_value(cfg->default_vhost->Set, hosts_reserved_names[2]);
        if((search_for_host_in_cfg(cfg, default_name)) == cfg->default_vhost)
            return NULL;
        return default_name;
    }

    return NULL;

}
int load_cfg(config_t* cfg)
{
    // this function opens files described as rootdir/indexfile and adds its file descriptors to config structure
    char* dir;
    char* file;
    char fullindexfile[256]={0};
    int fd;
    size_t len_dir;
    rb_tree_t* t;
    q_elem_t qe = NULL;
    q_elem_t tmp=NULL;
    Queue q;

    if(cfg->default_vhost != NULL)
    {
        dir=get_value(cfg->default_vhost->Set, hosts_reserved_names[0]);
        file=get_value(cfg->default_vhost->Set, hosts_reserved_names[1]);
        len_dir=strlen(dir);
        strncat(fullindexfile, dir, 127);
        if(dir[len_dir-1] != '/')
            fullindexfile[len_dir]='/';
        strncat(fullindexfile, file, 127);
        if((fd=open(fullindexfile,O_RDONLY)) == -1)
        {
            WriteLogPError(fullindexfile);
            return -1;
        }
        close(fd);
    }

    t=cfg->hosts;
    if(t == NULL)
        return 1;
    q=init_queue();
    qe=init_q_elem(t->root);
    push(q, qe);// pushing a root into queue

    while(!(pop(q, &tmp)))
    {
        dir=NULL;
        file=NULL;
        len_dir=0;
        memset(fullindexfile,0,sizeof(fullindexfile));

        if(tmp->node->left_node != NILL)
        {
            qe=init_q_elem(tmp->node->left_node);
            push(q, qe);
        }
        if(tmp->node->right_node != NILL)
        {
            qe=init_q_elem(tmp->node->right_node);
            push(q, qe);
        }

        dir=get_value(tmp->node->section->Set, hosts_reserved_names[0]);
        file=get_value(tmp->node->section->Set, hosts_reserved_names[1]);
        len_dir=strlen(dir);
        strncat(fullindexfile, dir, 127);


        if(dir[len_dir-1] != '/')
            fullindexfile[len_dir]='/';
        strncat(fullindexfile, file, 127);
        if((fd=open(fullindexfile,O_RDONLY)) == -1)
        {
            WriteLogPError(fullindexfile);
            return -1;
        }
        close(fd);
    }


    destroy_queue(q);

    return 0;
}
int reload_cfg(config_t* new_cfg, config_t* current_cfg, const char* filename)
{

    if((create_cfg_from_file(&new_cfg, filename)) == -1)
    {
        WriteLog("New configuration file was not applied. Continue working with previous");
        destroy_cfg(new_cfg);
        new_cfg=current_cfg;

        return -1;
    }
    WriteLog("New configuration file %s has been applied", filename);
    return 0;

}

int check_if_cfg_param_exists_in_section(section_t section, const char* param)
{
    char* value = NULL;
    value=get_value(section->Set, param);

    return value ? 1 : 0;
}
void print_section(section_t section)
{

    printf("%s\n", opening_section_names[section->type]);
    for(unsigned int i=0; i < section->Set->capacity;i++)
    {
        if(section->Set->array[i] != NULL)
        {
            printf("%s=",section->Set->array[i]->key);
            printf("%s\n",section->Set->array[i]->value);
        }
    }
    printf("%s\n", closing_section_names[section->type]);
}
void print_section_node(node_t* n)
{
    print_section(n->section);
}
void* action_node(node_t *n, void* args)
{
    args=NULL;//this is only for suppressing message "unused parameter args"
    print_section(n->section);
    return 0;
}

void print_cfg(config_t* cfg)
{
    print_section(cfg->general);
    if(cfg->default_vhost != NULL)
        print_section(cfg->default_vhost);

    if(cfg->hosts != NULL)
        level_order_traverseRBTree(cfg->hosts, action_node, NULL);
}
