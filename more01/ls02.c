
/*
    simulate ls command
    author dingtao
    date   14/09/2018

    Ver.2
    add buffer

*/

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include<sys/stat.h>

void show_stat_info(struct stat* );
void get_mode(int mode, char str[]);
char fmode[11];

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
    struct stat info ;

    if((ptr_dir = opendir(dirname))== NULL)
        fprintf(stderr,"can't open %s \n",dirname);
    else
    {//show dir
        printf("open %s \n",dirname);
        while((direntp = readdir(ptr_dir)) != NULL)
        {
            //printf("%s \n", direntp->d_name);
            if(stat(direntp->d_name, &info)== -1)
                perror("get file status error");
            else
            {   //show status
                show_stat_info(&info);

            }

        }
        closedir(ptr_dir);
    }


}

void show_stat_info(struct stat* st_info)
{

    printf("mode %o\n",st_info->st_mode);
    get_mode(st_info->st_mode,fmode);
    printf("mode %s \n",fmode);

}

void get_mode(int mode, char str[])
{
    strcpy(str, "----------");
    if(S_ISDIR(mode))
        str[0] = 'd';   //dir
    if(S_ISCHR(mode))
        str[0] = 'c';   //char device
    if(S_ISBLK(mode))
        str[0] = 'b';   //block device

    if(mode & S_IRUSR)
        str[1] = 'r';   //onwer
    if(mode & S_IWUSR)
        str[2] = 'w';   //onwer
    if(mode & S_IXUSR)
        str[3] = 'x';   //onwer

    if(mode & S_IRGRP)
        str[4] = 'r';   //group
    if(mode & S_IWGRP)
        str[5] = 'w';   //group
    if(mode & S_IXGRP)
        str[6] = 'x';   //group

    if(mode & S_IROTH)
        str[7] = 'r';   //other
    if(mode & S_IWOTH)
        str[8] = 'w';   //other
    if(mode & S_IXOTH)
        str[9] = 'x';   //other


}
