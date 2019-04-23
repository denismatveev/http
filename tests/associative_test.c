#include<stdio.h>
#include"associative.h"

int main(int argc, char** argv)
{
  char* str1="port";
  char* str2="root";
  assoc_t* arr;

  //listen=127.0.0.1;
  //workers=5;
  //timeout=300;

  unsigned long int hash1=hash_func(str1);
  unsigned long int hash2=hash_func(str2);

  unsigned long int ind1=get_index(hash1,2);
  unsigned long int ind2=get_index(hash2,2);

  printf("str1 = %s, hash = %lu, index = %lu\n", str1, hash1, ind1 );

  printf("str2 = %s, hash = %lu, index = %lu\n", str2, hash2, ind2);

  arr=init_assoc_array();


  add_elem(arr,"port","8080");
  add_elem(arr,"rootdir", "/home/denis");


  printf("port=%s\n", get_value(arr,"port"));

  printf("rootdir=%s\n", get_value(arr,"rootdir"));




  destroy_assoc_array(arr);

}
