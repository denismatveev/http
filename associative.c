#include<stdio.h>
#include"associative.h"
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
assoc_t* init_assoc_array(void)
{
    assoc_t* assoc;

    if((assoc=(assoc_t*)malloc(sizeof(assoc_t))) == NULL)
        return NULL;
    assoc->capacity=64;
    assoc->current_size=0;

    if((assoc->array=(tuple_t*)calloc(sizeof(tuple_t), assoc->capacity)) == NULL)
        return NULL;

    return assoc;
}
void destroy_assoc_array(assoc_t* arr)
{

    if(arr == NULL)
        return;
    for(unsigned int i=arr->capacity;i > 0;i--)
        destroy_tuple(arr->array[i]);

    free(arr->array);
    free(arr);

    return;
}

tuple_t init_tuple(const char key[128], const char value[128])
{
    tuple_t t;
    if((t=(tuple_t)malloc(sizeof(__tuple_t))) == NULL)
        return NULL;
    strncpy(t->key, key, 128);
    strncpy(t->value, value, 128);
    t->is_deleted=0;

    return t;
}

void destroy_tuple(tuple_t t)
{
    if(t == NULL)
        return;
    free(t);

    return;
}

unsigned long int hash_func(const char *k)
{

    char tmp[128];
    unsigned long int hash = 5381; // 5381 is a super-prime
    char s=0;
    int i=0;

    strncpy(tmp, k, 128);

    while ((s = tmp[i++]))
        hash = ((hash << 5) + hash) + (unsigned long)s; /* hash * 33 + c */

    return hash;
}

unsigned int calc_index(long unsigned int hash, unsigned int size)
{
    return (hash % size);
}


// functions
int add_elem(assoc_t *arr, const char* key, const char* value)
{
    // return error if array is full
    // because for expanding hash table it's required to create new hash table, then rehash each element
    if(arr->current_size == arr->capacity)
        return -1;
    unsigned long int hash;
    unsigned int i;
    tuple_t t;
    size_t val_len;
    val_len=strlen(value);

    if(val_len > 128 || val_len == 0)
        return -1;

    hash=hash_func(key);
    i=calc_index(hash, arr->capacity);

    if(arr->array[i] && arr->array[i]->is_deleted == 0)
    {
        // in case of collision (different keys hashed in the cell)
        // To resolve a collision we use linear approach

        for(;i < arr->capacity; i++)
        {
            if(arr->array[i] == NULL || arr->array[i]->is_deleted == 1)
                break;
            if(!(strncmp(arr->array[i]->key, key, 128))) // trying to add a tuple with not unique key
                return 1;
        }
        if(i == arr->capacity)
            for(i=0; i < arr->capacity; i++)
            {
                if(arr->array[i] == NULL || arr->array[i]->is_deleted == 1)
                    break;
                if(!(strncmp(arr->array[i]->key, key, 128)))
                    return 1;
            }

    }
    t=init_tuple(key, value);
    arr->array[i]=t;
    arr->current_size++;

    return 0;
}
char* get_value(const assoc_t* arr, const char *key)
{
    unsigned long int hash = hash_func(key);
    unsigned int i=calc_index(hash, arr->capacity);

    if(arr->array[i] && arr->array[i]->is_deleted == 0)
    {
        for(; i < arr->capacity; i++)
            if(!(strncmp(arr->array[i]->key, key, 128)))
                return arr->array[i]->value;

        for(i = 0; i < arr->capacity; i++)
            if(!(strncmp(arr->array[i]->key, key, 128)))
                return arr->array[i]->value;

    }

    return  NULL;
}


int remove_elem(assoc_t* arr, const char* key)
{
    // deleting an element from a hash table requires some procedure to leave all keys hashed in one cell searchable
    unsigned long int hash = hash_func(key);
    unsigned int i=calc_index(hash, arr->capacity);


    if(arr->array[i] && arr->array[i]->is_deleted == 0)
    {
        for(; i < arr->capacity; i++)
            if(!(strncmp(arr->array[i]->key, key, 128)))
                break;
        if(i == arr->capacity)
            for(i = 0; i < arr->capacity; i++)
                if(!(strncmp(arr->array[i]->key, key, 128)))
                    break;

    }
    else
        return 1;

    arr->array[i]->is_deleted = 1;
    arr->current_size--;

    return 0;
}
