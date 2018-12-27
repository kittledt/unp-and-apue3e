
/*
    simulate pwd command
    author dingtao
    date   14/09/2018

    Ver.1


    current dir . opendir  . node.
    paraent node


*/

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include<sys/stat.h>

// get i-node for dir
ino_t get_inode(char * filename)
{
    struct stat info;
    if(stat(filename,&info) == -1)
    {
        fprintf(stderr,"can't stat");
        perror(filename);
        exit(1);

    }
    printf("inode %d \n",info.st_ino);
    return info.st_ino;

}

void inode2name(ino_t inode_to_find, char * buf, int buflen)
{
    // input inode, find dirname.
    DIR* ptr_dir = NULL;
    struct dirent* direntp = NULL;
    if((ptr_dir = opendir("."))== NULL)
    {
        fprintf(stderr,"can't open %s \n",".");
        perror(".");
        exit(1);

    }

    while((direntp = readdir(ptr_dir)) != NULL)
        {

            if(direntp->d_ino == inode_to_find)
            {
                printf("dir name %s \n",direntp->d_name);
                strncpy(buf, direntp->d_name,buflen);
                buf[strlen(buf)] = '\0';
                closedir(ptr_dir);
                return;

            }

        }
        fprintf(stderr,"error looking for inode %d \n",inode_to_find);
        exit(1);
}


void printpath(ino_t cur_inode)     // . curent dir
{

    ino_t my_inode;
    char path_name[BUFSIZ];
    if(get_inode("..") != cur_inode)
    {
        chdir("..") ;    //go into parent dir
        inode2name(cur_inode,path_name,BUFSIZ);
        my_inode = get_inode(".");
        printpath(my_inode);        //recursive to old  current dir's parent as   new current
        printf("/%s",path_name);

    }
    // else,  inode(..) == inode(.) to top /


}

int main ()
{
    printpath(get_inode("."));
    putchar('\n');
    return 0;

}

