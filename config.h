#ifndef CONFIG_TREE_H
#define CONFIG_TREE_H
#include<stdio.h>
#include<string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include"associative.h"

#define INVALID_GENERAL_KEY -1
#define INVALID_HOSTS_KEY -2
#define INVALID_SECTION_NAME -3
typedef struct _node node_t;
extern node_t nullnode;
#define NILL (&nullnode)

extern char *opening_section_names[];
extern char *closing_section_names[];

#define HOSTS_RESERVED_NAMES(XX)                       \
        XX(0, ROOTDIR, rootdir                        )\
        XX(1, INDEXFILE, indexfile                    )\
        XX(2, SERVERNAME, servername                  )\

typedef enum hosts_reserved_names
{
#define XX(num, name, hosts_reserved_names) HOSTS_RESERVED_NAMES_##name = num,
    HOSTS_RESERVED_NAMES(XX)
#undef XX
}hosts_reserved_names_t;

#define GENERAL_RESERVED_NAMES(XX)                 \
    XX(0, PORT, port                              )\
    XX(1, LISTEN, listen                          )\
    XX(2, WORKERS, workers                        )\
    XX(3, TIMEOUT, timeout                        )
typedef enum
{
#define XX(num, name, general_reserved_names) GENERAL_RESERVED_NAMES_##name = num,
    GENERAL_RESERVED_NAMES(XX)
#undef XX
}general_reserved_names_t;

extern char *hosts_reserved_names[];
extern char *general_reserved_names[];
typedef enum section_type
{
    Section_General=0,// General section contains info such as port, timeout, workers,listening interface etc
    Section_Default=1,// this config is used if any of 'Host' config is not suitable
    Section_Host=2 // Host config describes virtual host
}section_type_t;
//TODO replace assoc_t by the struct below
/*
typedef struct __hosts_section_data
{
    char rootdir[256];
    char indexfile[256];
    char servername[256];
    uint16_t port;
}hosts_section_data_t;
typedef struct __general_section_data
{
    char workers;
    unsigned int timeout;
    struct in_addr listern_addr;
}general_section_data_t;
*/
typedef struct __section
{
    section_type_t type;
    assoc_t* Set;
    int indexfile_fd;
}__section_t;

typedef __section_t* section_t;
typedef struct red_black_tree rb_tree_t;
typedef struct config
{
    rb_tree_t *hosts;
    section_t general;
    section_t default_vhost;
}config_t;
extern config_t* cfg;
/* Red Black tree structures */
typedef enum color
{
    Black=0,
    Red=1
}color_t;

typedef struct _node
{
    char hostname[256]; // key for searching is 'char* hostname' and also it is a comparison field
    section_t section;
    //TODO add a comparator with void*
    //void* section; // if type is void*, RBTree can store any data
    color_t color;
    struct _node* parent;
    struct _node* right_node;
    struct _node* left_node;
}node_t;


typedef struct red_black_tree
{
    node_t* root;
}rb_tree_t;
typedef struct __params
{
    char * sitename;
    char * param;
}params_t;

/* Red Black tree related functions */
node_t* init_node(const char *host, const char *rootdir, const char *index);
node_t* init_node_s(section_t s);
void destroy_node(node_t*);
rb_tree_t* init_rbtree(node_t *node);

void destroy_rbtree(rb_tree_t*);

void rbtree_delete(rb_tree_t *rbtree, node_t *data);
node_t *insert_in_rbtree(rb_tree_t* rbtree, node_t* n);
node_t *rbtree_search ( rb_tree_t *rbtree, const char *hostname);
node_t* search_host(rb_tree_t* tree, const char* host);

// for debugging
void print_node(node_t*);
void printRBTree(rb_tree_t *tree);
/* config related functions */
section_t init_section(section_type_t t);
int fill_section(section_t s, const char* key, const char* value);
void destroy_section(section_t t);
int create_cfg_from_file(config_t **cfg, const char* fname);
config_t* init_cfg(void);
void destroy_cfg(config_t*);
int parse_str(char* wholestr, char *key, char *value, char comment, char delim, char ending);
long get_section_data_from_config_file(section_t *section, char *sitename, FILE* f);
int read_config_section_from_file(FILE* f, section_t* section, char* sitename, int *section_counter, section_type_t s);
int check_cfg(config_t* cfg);
int load_cfg(config_t* cfg);
char *check_cfg_for_duplicated_sitename(config_t* cfg);
int check_if_cfg_param_exists_in_section(section_t section, const char *param);
section_t search_for_host_in_cfg(const config_t *c, const char* host);
int check_section(section_t section, char** param);
void print_cfg(config_t* cfg);
void print_section(section_t section);
void print_node(node_t* n);
void* action_node(node_t* node, void*);
int check_section_in_RBtree(node_t*, void*);
void level_order_traverseRBTree(rb_tree_t *t, void*(*action)(node_t*, void*), void *);


#endif //CONFIG_TREE_H
