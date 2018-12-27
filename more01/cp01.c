/*
    copy file

*/
#include<stdio.h>
#include <stdlib.h>
#include<unistd.h>   /*file read close*/
#include<fcntl.h>   /* file open, create*/
#include<time.h>

#define BUFSIZE 4096
#define COPYMODE 0644    /* rw- r-- r-- */
main(int argc, char *argv[])
{
    int src_fd= 0 , dst_fd = 0;
    int buf[BUFSIZE];
    int size_rd;    /* read size actually */
    int size_wr;    /* write size actually */
    /* parse params src, dst*/
    if(argc != 3)
    {
        /*  cp01 , src path, dst path */
        printf("should have src and dst filename\n");
        exit(1);
    }
    /*open src file*/
    src_fd= open( argv[1],O_RDONLY);
    if(src_fd == -1)
    {
        /*open src file failed*/
        printf("open src file error\n");
        exit(1);
    }
    /*open dst file, if not exist, then create*/
    dst_fd= creat( argv[2],COPYMODE);
    if(dst_fd == -1)
    {
        /*open src file failed*/
        printf("open dst file error\n");
        exit(1);
    }
    /* read num to buf   */
    while(size_rd =  read(src_fd, buf,BUFSIZE))
    {
        /* write  */

        size_wr = write(dst_fd,buf,size_rd);     /* last time number maybe not = Bufsize */
        if(size_wr == -1)
        {
            /*open src file failed*/
            printf("write dst file error\n");
            exit(1);
        }
        else if(size_wr != size_rd)
        {
            /* disk full */
            printf("disk is full!\n");
            exit(1);
        }

    }

    /*close file*/
    close(src_fd);
    close(dst_fd);

}
