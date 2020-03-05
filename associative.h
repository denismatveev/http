#ifndef ASSOCIATIVE_ARRAY_H_
#define ASSOCIATIVE_ARRAY_H_
#include<stdlib.h>

typedef struct __tuple
{
  char key[128];
  char value[128];
  char is_deleted;
} __tuple_t;
typedef __tuple_t* tuple_t;

tuple_t init_tuple(const char key[128], const char value[128]);
void destroy_tuple(tuple_t);

typedef struct _assocArray
{
  tuple_t *array;
  unsigned int capacity;
  unsigned int current_size;
} assoc_t;

assoc_t* init_assoc_array(void);
void destroy_assoc_array(assoc_t* arr);

// functions
int add_elem(assoc_t* arr, const char *key, const char* value);
char* get_value(const assoc_t* arr, const char* key);
int remove_elem(assoc_t* arr, const char* key);

long unsigned int hash_func(const char* k);
unsigned int calc_index(long unsigned int hash, unsigned int size);

#endif //




// <General>
//port = 8080;
//listen =127.0.0.1;
//workers =5;#test
//timeout = 300;
//</General>
//<Host>
//name=denis.ilmen-tau.ru
//rootdir=/var/www/denis/
//indexfile=index.html
//</Host>
//<Host>
//name=nastya.ilmen-tau.ru
//rootdir=/var/www/nastya
//indexfile=index.html
//</Host>

