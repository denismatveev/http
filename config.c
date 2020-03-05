#include<string.h>
#include<stdlib.h>
#include"config.h"
#include"queue.h"

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

section_t init_section(section_type_t t)
{
    section_t s;
    assoc_t *arr;

    arr=init_assoc_array();

    if((s=(section_t)malloc(sizeof (__section_t))) == NULL)
        return NULL;
    s->type = t;
    //s->Set = (assoc_t*)&arr;
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

