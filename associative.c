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
  arr->capacity=0;
  free(arr->array);
  free(arr);

  return;
}

tuple_t init_tuple(char key[128], char value[128])
{
  tuple_t t;
  t=(tuple_t)malloc(sizeof(__tuple_t));
  strncpy(t->key, key, 128);
  strncpy(t->value, value, 128);

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
  unsigned long int hash = 5381;
  char s=0;
  int i=0;

  strncpy(tmp, k, 128);

  while (s = tmp[i++])
    hash = ((hash << 5) + hash) + s; /* hash * 33 + c */

  return hash;
}

unsigned int get_index(long unsigned int hash, unsigned int size)
{
  return (hash % size);
}


int reserve_array(assoc_t* arr, unsigned int size)
{
  tuple_t* new_tuple_arr;
  tuple_t* tmp;

  if( size < arr->capacity)
    return -1;

  if((new_tuple_arr=(tuple_t*)realloc(arr->array, size*sizeof(tuple_t))) == NULL)
    return -2;

  tmp=arr->array;

  arr->array=new_tuple_arr;
  for(unsigned int i=0; i < arr->capacity;i++)
    new_tuple_arr[i]=tmp[i];

  arr->capacity=size;

  free(tmp);
  return 0;
}
// functions
int add_elem(assoc_t *arr, const char* key, const char* value)
{
  // TODO add collisions resolving
  unsigned long int hash;
  unsigned int i;
  tuple_t t;
  size_t val_len;
  val_len=strlen(value);

  if(val_len > 128 || val_len == 0)
    return -1;

  hash=hash_func(key);
  i=get_index(hash, arr->capacity);

  t=init_tuple(key, value);

  arr->array[i]=t;

  return 0;


}

char* get_value(const assoc_t* arr, const char *key)
{

  unsigned long int hash = hash_func(key);
  unsigned int i=get_index(hash, arr->capacity);

  return arr->array[i]->value;
}
