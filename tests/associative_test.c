#include<stdio.h>
#include"associative.h"

void printAssocArray(assoc_t* arr)
{
    for(unsigned int i = 0; i < arr->capacity; i++)
        if(arr->array[i] != NULL && arr->array[i]->is_deleted == 0)
            printf("key=%s, value=%s, index=%d\n", arr->array[i]->key, arr->array[i]->value,i);

}


int main(int argc, char** argv)
{
    // test 1
    char* str1="port";
    char* str2="rootdir";
    char* str3="listen";
    char* str4="timeout";
    char* str5="workers";
    unsigned int size=64;
    assoc_t* arr;

    unsigned long int hash1=hash_func(str1);
    unsigned long int hash2=hash_func(str2);
    unsigned long int hash3=hash_func(str3);
    unsigned long int hash4=hash_func(str4);
    unsigned long int hash5=hash_func(str5);

    unsigned long int ind1=calc_index(hash1,size);
    unsigned long int ind2=calc_index(hash2,size);
    unsigned long int ind3=calc_index(hash3,size);
    unsigned long int ind4=calc_index(hash4,size);
    unsigned long int ind5=calc_index(hash5,size);


    printf("str1 = %s, hash = %lu, index = %lu\n", str1, hash1, ind1 );
    printf("str2 = %s, hash = %lu, index = %lu\n", str2, hash2, ind2);
    printf("str3 = %s, hash = %lu, index = %lu\n", str3, hash3, ind3);
    printf("str4 = %s, hash = %lu, index = %lu\n", str3, hash3, ind4);
    printf("str5 = %s, hash = %lu, index = %lu\n", str4, hash5, ind5);
    arr=init_assoc_array();


    add_elem(arr,str1,"8080");
    add_elem(arr,str2, "/home/denis");
    add_elem(arr,str3, "0.0.0.0");
    add_elem(arr,str4, "30");
    add_elem(arr,str5, "5");


    printf("\nPrinting all elements in arr1:\n");
    //  printAssocArray(arr);
    printf("port=%s\n", get_value(arr,"port"));
    printf("rootdir=%s\n", get_value(arr,"rootdir"));
    printf("listen=%s\n", get_value(arr,"listen"));
    printf("timeout=%s\n", get_value(arr,"timeout"));
    printf("worker=%s\n", get_value(arr,"worker"));
    printf("Removing one element\n");
    remove_elem(arr, "port");
    printAssocArray(arr);

    destroy_assoc_array(arr);
    //test2
    // A=A
    // AB=AB
    // ABC=ABC
    // ABCD=ABCD
    // etc
    assoc_t* arr2;
    arr2=init_assoc_array();
    char str[128];
    unsigned int i,k;

    for(i = 0,k = 65; i < 40; i++,k++)// array element
    {
        if( k == 91)
            k=65;
        str[i]=(char)(k);

        str[i+1] = 0;

        add_elem(arr2, str , str);

    }

    printf("\nPrinting array arr2:\n");
    printAssocArray(arr2);
    destroy_assoc_array(arr2);
    // test3

    assoc_t* arr3;
    arr3=init_assoc_array();




    add_elem(arr3,"abc","abc");
    remove_elem(arr3,"abc"); // removing an element


    // adding different keys that have identical hash

    add_elem(arr3,"ABCDEFGHIJKLMNOPQRSTUVWXYZABC","value");
    add_elem(arr3, "port","8080");
    add_elem(arr3,"port","9080"); // try to add element with not unique key
    remove_elem(arr3,"port");
    add_elem(arr3, "port","80");
    printf("Prining arr3:\n");
    printAssocArray(arr3);
    destroy_assoc_array(arr3);
}

