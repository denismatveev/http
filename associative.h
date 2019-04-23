#ifndef ASSOCIATIVE_ARRAY_H_
#define ASSOCIATIVE_ARRAY_H_


typedef struct __tuple
{
  char key[128];
  char value[128];
} __tuple_t;
typedef __tuple_t* tuple_t;

tuple_t init_tuple(char key[128], char value[128]);
void destroy_tuple(tuple_t);

typedef struct _assocArray
{
  tuple_t *array;
  unsigned int capacity;
} assoc_t;

assoc_t* init_assoc_array(void);
void destroy_assoc_array(assoc_t* arr);
int reserve_array(assoc_t* arr, unsigned int size);
// functions
int add_elem(assoc_t* arr, const char *key, const char* value);
char* get_value(const assoc_t* arr, const char* key);
int remove_value(assoc_t* arr, const char* value);


long unsigned int hash_func(const char* k);

unsigned int get_index(long unsigned int hash, unsigned int size);


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

