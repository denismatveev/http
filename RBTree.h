#ifndef RBTREE_H_
#define RBTREE_H_
#include"config.h"
/* Tree related functions and structures */
typedef enum color
{
    Black=0,
    Red=1
}color_t;

typedef struct _node
{

    char hostname[256]; // key for searching is 'char* hostname' and also it is a comparison field
    section_t section;
    color_t color;
    struct _node* parent;
    struct _node* right_node;
    struct _node* left_node;
}node_t;

extern node_t nullnode;
#define NILL (&nullnode)

typedef struct red_black_tree
{
    node_t* root;
}rb_tree_t;
typedef rb_tree_t* config_t;

node_t* init_node(const char *host, const char *rootdir, const char *index);
void destroy_node(node_t*);
rb_tree_t* init_rbtree(node_t *node);
void destroy_rbtree(rb_tree_t*);



void rbtree_delete(rb_tree_t *rbtree, node_t *data);
node_t *insert_in_rbtree(rb_tree_t* rbtree, node_t* n);
node_t *rbtree_search ( rb_tree_t *rbtree, char *hostname);


// for debugging
void print_node(node_t*);
void printRBTree(rb_tree_t *tree);

#endif // __RBTREE_H_
