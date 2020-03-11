#include "config.h"

int main(int argc, char** argv)
{
    node_t* node;
    rb_tree_t* tree;
    int size;

    char* hosts[]={"5", "3", "2", "4", "6", "1", "7", "9", "8"};
    node = init_node(hosts[0], "/var/www", "index.html");
    tree = init_rbtree(node);
    node_t* snode;
    char hostname[3]="5";

    size = sizeof(hosts)/sizeof (char*);
    for(int i = 1; i < size; i++)
    {

        node=init_node(hosts[i], "/var/www", "index.html");
        printf("inserting %s\n", hosts[i]);
        insert_in_rbtree(tree, node);
        printRBTree(tree);

    }
    printf("\n");
    printf("Deleting %s from hosts4\n", hostname);

    snode=rbtree_search(tree, hostname);
    if(snode != NILL)
        rbtree_delete(tree, snode);
    printRBTree(tree);


    for(int i=8; i > 0; i--)
    {
        printf("Deleting %s\n", hosts[i]);
        snode=rbtree_search(tree, hosts[i]);
        if(snode != NILL)
            rbtree_delete(tree, snode);
        printRBTree(tree);
    }
    printf("Deleting 5\n" );
    snode=rbtree_search(tree, "5");
    if(snode != NILL)
        rbtree_delete(tree, snode);
    printRBTree(tree);

    destroy_rbtree(tree);



    //1,2,3,4,5,6,7,8,9,10

}
