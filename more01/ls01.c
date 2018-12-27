/*
    simulate who command
    author dingtao
    date   13/09/2018

    Ver.2
    add buffer

*/

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char *argv[])
{

    if (argc == 1)          /* current dir*/
        do_ls(".");



    while(--argc)         //read dir one by one
    {
        printf("%s:\n",*++argv);
        do_ls(*argv);

    }


    //close dir


}

void do_ls(char * dirname)
{
    /*open dir*/
    DIR* ptr_dir = NULL;
    struct dirent* direntp = NULL;

    if((ptr_dir = opendir(dirname))== NULL)
        fprintf(stderr,"can't open %s \n",dirname);
    else
    {//show dir
        printf("open %s \n",dirname);
        while((direntp = readdir(ptr_dir)) != NULL)
            printf("%s \n", direntp->d_name);
        closedir(ptr_dir);
    }


}
