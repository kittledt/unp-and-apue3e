
#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<dirent.h>
#include<stdlib.h>
#include<string.h>

#define SIZE 128

ino_t get_inode(char *dirname);
void get_work_dir(ino_t inode_num);
void inode_to_dirname(ino_t inode_num, char *buf, int buflen);

int main(void)
{
    get_work_dir(get_inode("."));
    printf("\n");
    return 0;

}

ino_t get_inode(char *dirname)
{
    struct stat info;
    if (stat(dirname, &info) == -1)
    {
        perror("dirname");
        exit(1);
    }
    printf("inode %d \n",info.st_ino);
    return info.st_ino;
}

void get_work_dir(ino_t inode_num)
{
    ino_t parent_inode;
    char buf[SIZE];
    if (get_inode("..") != inode_num)
    {
        chdir("..");
        inode_to_dirname(inode_num, buf, SIZE);
        parent_inode = get_inode(".");
        get_work_dir(parent_inode);
        printf("/%s", buf);
    }
}

void inode_to_dirname(ino_t inode_num, char *buf,int buflen)
{
    DIR *dir_ptr;
    struct dirent *dire;
    if ((dir_ptr = opendir(".")) == NULL)
    {
        perror(".");
        exit(1);
    }

    while ((dire = readdir(dir_ptr)) != NULL)
    {
        if (dire->d_ino == inode_num)
        {
            strncpy(buf, dire->d_name, buflen);
            buf[strlen(buf)] = '\0';
            closedir(dir_ptr);
            return ;
        }
    }
    fprintf(stderr, "error looking for inode number %d\n", (int)inode_num);
    exit(1);
}
