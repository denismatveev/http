#include<stdio.h>
#include"config.h"


int main(int argc, char** argv)
{
    config_t* cfg=NULL;

    if((create_config(&cfg, argv[1])))
            printf("Something went wrong. Check logs\n");

    destroy_config(cfg);

}
