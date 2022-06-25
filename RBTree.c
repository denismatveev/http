#include"queue.h"

#pragma GCC diagnostic ignored "-Wuninitialized"
// NILL node
node_t nullnode = { {0}, NULL, Black, NULL, NULL, NULL };

// init_node() function is initializing RB tree node with section inside. The key of the node is hostname
node_t* init_node(const char *name, const char* rootdir, const char* index)
{
    node_t* node;
    section_t s;

    if(name == NULL)
        return NULL;

    if(strlen(name) == 0)
        return NULL;

    s=init_section(Section_Host);

    if((node=(node_t*)malloc(sizeof(node_t))) == NULL)
        return NULL;

    strncpy(node->hostname, name, 256);
    node->section=s;
    if(fill_section(s, "rootdir", rootdir))
        goto exit;
    if(fill_section(s,"index", index))
        goto exit;

    //    node->section=NULL;
    node->color=Red;    // default colour is Red
    node->left_node=NILL;
    node->right_node=NILL;
    node->parent=NILL;
    return node;
exit:
    destroy_section(s);
    free(node);
    return NULL;
}

node_t* init_node_s(section_t s)
{
    node_t* node;
    if((node=(node_t*)malloc(sizeof(node_t))) == NULL)
        return NULL;
    memset(node->hostname,0,sizeof(node->hostname));
    node->section=(section_t)s;
    node->color=Red;    // default colour is Red
    node->left_node=NILL;
    node->right_node=NILL;
    node->parent=NILL;

    return node;
}

void destroy_node(node_t* node)
{
    if(node == NULL || node == NILL)
        return;
    destroy_section(node->section);
    free(node);

    return;
}


// init_tree() function is initializing  tree with pointer to the root
rb_tree_t* init_rbtree(node_t* node)
{

    rb_tree_t* tr;

    if((tr=(rb_tree_t*)malloc(sizeof (rb_tree_t))) == NULL)
        return NULL;

    tr->root=node;
    tr->root->color=Black; // root is always Black

    return tr;

}
// destroy_tree() destroys all allocated nodes. To travers the nodes a queue is used. Initially the root is being put into queue, then the algorithm is putting all children if the root into queue(if they are not NULL)
// it is Breadth-first search (BFS)
// Queue implemeted as linked list
void destroy_rbtree(rb_tree_t* t)
{
    q_elem_t qe;
    q_elem_t tmp=NULL;

    if(t == NULL)
        return;
    Queue q=init_queue();
    qe=init_q_elem(t->root);
    push(q, qe);// pushing a root into queue

    while(!(pop(q, &tmp)))
    {
        if(tmp->node->left_node != NULL)
        {
            qe=init_q_elem(tmp->node->left_node);
            push(q, qe);
        }
        if(tmp->node->right_node != NULL)
        {
            qe=init_q_elem(tmp->node->right_node);
            push(q, qe);
        }

        destroy_q_elem(tmp);
    }
    destroy_queue(q);
    free(t);
    return;
}

void rbtree_rotate_left(rb_tree_t *rbtree, node_t *node)
{
    node_t *right = node->right_node;

    node->right_node = right->left_node;
    if (right->left_node != NILL)
        right->left_node->parent = node;

    right->parent = node->parent;

    if (node->parent != NILL)
    {
        if (node == node->parent->left_node)
            node->parent->left_node = right;
        else
            node->parent->right_node = right;
    }
    else
        rbtree->root = right;
    right->left_node = node;
    node->parent = right;
}

void rbtree_rotate_right(rb_tree_t *rbtree, node_t *node)
{
    node_t *left = node->left_node;

    node->left_node = left->right_node;
    if (left->right_node != NILL)
        left->right_node->parent = node;

    left->parent = node->parent;

    if (node->parent != NILL)
    {
        if (node == node->parent->right_node)
            node->parent->right_node = left;
        else
            node->parent->left_node = left;
    }
    else
        rbtree->root = left;
    left->right_node = node;
    node->parent = left;
}


void rbtree_insert_balance(rb_tree_t *rbtree, node_t *node)
{
    node_t *uncle;

    while (node != rbtree->root && node->parent->color == Red)
    {
        if (node->parent == node->parent->parent->left_node)
        {
            uncle = node->parent->parent->right_node;

            if (uncle->color == Red)
            {
                node->parent->color = Black;
                uncle->color = Black;
                node->parent->parent->color = Red;
                node = node->parent->parent;
            }
            else
            {
                if (node == node->parent->right_node)
                {
                    node = node->parent;
                    rbtree_rotate_left(rbtree, node);
                }
                node->parent->color = Black;
                node->parent->parent->color = Red;
                rbtree_rotate_right(rbtree, node->parent->parent);
            }
        }
        else
        {
            uncle = node->parent->parent->left_node;

            if (uncle->color == Red)
            {
                node->parent->color = Black;
                uncle->color = Black;
                node->parent->parent->color = Red;
                node = node->parent->parent;
            }
            else
            {
                if (node == node->parent->left_node)
                {
                    node = node->parent;
                    rbtree_rotate_right(rbtree, node);
                }
                node->parent->color = Black;
                node->parent->parent->color = Red;
                rbtree_rotate_left(rbtree, node->parent->parent);
            }
        }
    }
    rbtree->root->color = Black;
}



void change_parent_ptr(rb_tree_t* rbtree, node_t* parent, node_t* old, node_t* new)
{
    if(parent == NILL)
    {
        if(rbtree->root == old) rbtree->root = new;
        return;
    }
    if(parent->left_node == old) parent->left_node = new;
    if(parent->right_node == old) parent->right_node = new;
}

node_t* insert_in_rbtree(rb_tree_t *rbtree, node_t *n)
{
    if(rbtree == NULL || n == NULL)
        return NULL;
    int r = 0;
    node_t *node = rbtree->root;
    node_t *parent = NILL;

    while (node != NILL)
    {
        parent = node;
        r = strncmp(n->hostname, node->hostname, 256);

        if (r < 0)
            node = node->left_node;
        else if(r > 0)
            node = node->right_node;
        else return NULL; // if such node already exist(duplicated)
    }

    n->parent = parent;
    n->left_node = n->right_node = NILL;
    n->color = Red;

    if (parent != NILL)
    {
        if (r < 0)
            parent->left_node = n;
        else
            parent->right_node = n;
    }
    else
    {
        rbtree->root = n;
    }
    rbtree_insert_balance(rbtree, n);
    return NILL;
}

void swap_np(node_t** x, node_t** y)
{
    node_t* t = *x; *x = *y; *y = t;
}

void change_child_ptr(node_t* child, node_t* old, node_t* new)
{
    if(child == NILL)
        return;
    if(child->parent == old)
        child->parent = new;
}

void rbtree_delete_balance(rb_tree_t* rbtree, node_t* child, node_t* child_parent)
{
    node_t* sibling;
    int go_up = 1;

    if(child_parent->right_node == child)
        sibling = child_parent->left_node;
    else
        sibling = child_parent->right_node;

    while(go_up)
    {
        if(child_parent == NILL)
            return;

        if(sibling->color == Red)
        {
            child_parent->color = Red;
            sibling->color = Black;
            if(child_parent->right_node == child)
                rbtree_rotate_right(rbtree, child_parent);
            else
                rbtree_rotate_left(rbtree, child_parent);
            if(child_parent->right_node == child)
                sibling = child_parent->left_node;
            else
                sibling = child_parent->right_node;
        }

        if(child_parent->color == Black
                && sibling->color == Black
                && sibling->left_node->color == Black
                && sibling->right_node->color == Black)
        {
            if(sibling != NILL)
                sibling->color = Red;
            child = child_parent;
            child_parent = child_parent->parent;
            if(child_parent->right_node == child)
                sibling = child_parent->left_node;
            else
                sibling = child_parent->right_node;
        }
        else
            go_up = 0;
    }

    if(child_parent->color == Red
            && sibling->color == Black
            && sibling->left_node->color == Black
            && sibling->right_node->color == Black)
    {
        if(sibling != NILL)
            sibling->color = Red;
        child_parent->color = Black;
        return;
    }

    if(child_parent->right_node == child
            && sibling->color == Black
            && sibling->right_node->color == Red
            && sibling->left_node->color == Black)
    {
        sibling->color = Red;
        sibling->right_node->color = Black;
        rbtree_rotate_left(rbtree, sibling);
        if(child_parent->right_node == child)
            sibling = child_parent->left_node;
        else
            sibling = child_parent->right_node;
    }
    else if(child_parent->left_node == child
            && sibling->color == Black
            && sibling->left_node->color == Red
            && sibling->right_node->color == Black)
    {
        sibling->color = Red;
        sibling->left_node->color = Black;
        rbtree_rotate_right(rbtree, sibling);
        if(child_parent->right_node == child)
            sibling = child_parent->left_node;
        else
            sibling = child_parent->right_node;
    }

    sibling->color = child_parent->color;
    child_parent->color = Black;
    if(child_parent->right_node == child)
    {
        sibling->left_node->color = Black;
        rbtree_rotate_right(rbtree, child_parent);
    }
    else
    {
        sibling->right_node->color = Black;
        rbtree_rotate_left(rbtree, child_parent);
    }
}

void rbtree_delete(rb_tree_t *rbtree, node_t *to_delete)
{
    node_t *child;
    color_t orig_color;

    if(to_delete->parent == NULL)
        return;

    if(to_delete->right_node != NILL && to_delete->right_node != NILL)
    {
        node_t *smright = to_delete->right_node;

        while(smright->left_node != NILL)
            smright = smright->left_node;

        orig_color=to_delete->color;
        to_delete->color=smright->color;
        smright->color=orig_color;

        change_parent_ptr(rbtree, to_delete->parent, to_delete, smright);
        if(to_delete->right_node != smright)
            change_parent_ptr(rbtree, smright->parent, smright, to_delete);

        change_child_ptr(smright->left_node, smright, to_delete);
        change_child_ptr(smright->left_node, smright, to_delete);
        change_child_ptr(smright->right_node, smright, to_delete);
        change_child_ptr(smright->right_node, smright, to_delete);
        change_child_ptr(to_delete->left_node, to_delete, smright);
        if(to_delete->right_node != smright)
            change_child_ptr(to_delete->right_node, to_delete, smright);
        if(to_delete->right_node == smright)
        {
            to_delete->right_node = to_delete;
            smright->parent = smright;
        }

        swap_np(&to_delete->parent, &smright->parent);
        swap_np(&to_delete->left_node, &smright->left_node);
        swap_np(&to_delete->right_node, &smright->right_node);
    }

    if(to_delete->left_node != NILL)
        child = to_delete->left_node;
    else
        child = to_delete->right_node;

    change_parent_ptr(rbtree, to_delete->parent, to_delete, child);
    change_child_ptr(child, to_delete, to_delete->parent);

    if(to_delete->color == Red)
        ;
    else if(child->color == Red)
    {
        if(child!=NILL)
            child->color = Black;
    }
    else
        rbtree_delete_balance(rbtree, child, to_delete->parent);

}

node_t *rbtree_search (rb_tree_t *rbtree, const char *hostname)// hostname is a key for searching for node in RB Tree
{
    int r = 0;
    node_t *node = rbtree->root;

    while (node != NILL)
    {
        r = strncmp(hostname,node->hostname, 256);
        if(r == 0)
            return node;
        else if (r < 0)
            node = node->left_node;
        else
            node = node->right_node;
    }
    return NILL;
}

node_t* search_host(rb_tree_t* tree, const char* host)//host is a key for searching for node with such host
{
    node_t* n;
    if((n=rbtree_search(tree, host)) == NILL)
        return NULL;

    return n;
}

void print_node(node_t* node)
{
    if(node == NULL)
        return;
    if(node == NILL)
        printf("| N |");
    else
    {
        if(node->color == Red)
            printf("\033[1;31m");
        printf("| %s |", node->hostname);
        printf("\033[0m");// print red color in terminal
    }
    return;
}


void printRBTree(rb_tree_t *t)
{
    q_elem_t qe;
    q_elem_t tmp=NULL;

    if(t == NULL)
        return;
    Queue q=init_queue();
    qe=init_q_elem(t->root);
    qe->is_end=1;
    push(q, qe);// pushing the root into queue

    while(!pop(q, &tmp))
    {
        if(tmp->node->left_node != NULL)
        {
            qe=init_q_elem(tmp->node->left_node);
            push(q, qe);
        }
        if(tmp->node->right_node != NULL)
        {
            qe=init_q_elem(tmp->node->right_node);
            if(tmp->is_end == 1)
                qe->is_end=1;
            push(q,qe);
        }

        print_node(tmp->node);
        if(tmp->is_end == 1)
            printf("\n");

        if(q->first == NULL || q->last == NULL)
            printf("\n");
    }
    destroy_q_elem(tmp);

    destroy_queue(q);
}

void level_order_traverseRBTree(rb_tree_t *t, void* (*action)(node_t*, void*), void* args)
{
    /* this is level order traversal or breadth-first search, BFS*/

    q_elem_t qe = NULL;
    q_elem_t tmp=NULL;
    Queue q;

    q=init_queue();
    qe=init_q_elem(t->root);
    push(q, qe);// pushing a root into queue

    while(!(pop(q, &tmp)))
    {


        if(tmp->node == NILL)// skipping pushing NILL nodes into queue
            continue;

        if((action(tmp->node, args)) != 0)
            break;

        if(tmp->node->left_node != NULL)
        {
            qe=init_q_elem(tmp->node->left_node);
            push(q, qe);
        }
        if(tmp->node->right_node != NULL)
        {
            qe=init_q_elem(tmp->node->right_node);
            push(q, qe);
        }
    }
    destroy_queue(q);
}
