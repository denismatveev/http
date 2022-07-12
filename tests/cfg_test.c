#include<stdio.h>
#include"config.h"


int main(int argc, char** argv)
{
    config_t* cfg=NULL;

    section_t section = NULL;
    if((create_cfg_from_file(&cfg, argv[1])))
    {
        printf("Wrong config. Check logs\n");
        exit(1);

    }
    if((check_cfg(cfg) < 0))
    {
        printf("Wrong config. Check logs\n");
        exit(1);
    }

    section=search_for_host_in_cfg(cfg, "site2.example.com");
    if(section == NULL)
    {
        printf("Not found\n");
        exit(1);
    }
    printf("\n\n");
    printf("Found in section %s:\n",opening_section_names[section->type]);
    print_section(section);

    printf("\n\n");
    print_cfg(cfg);

    destroy_cfg(cfg);

}
