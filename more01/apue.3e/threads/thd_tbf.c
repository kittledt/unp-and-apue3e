/*******************888

     ./thd_tbf /etc/services  num

     num is worker thread number

*/
//main.c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include "mytbf.h"
int main(int argc,char *argv[])
{
    int readfd[MYTBF_MAX];
    int num;

    // open 1 file many times in this process.
    if(argc != 3)
    {
        fprintf(stderr,"Usage:%s <src_file> <num>\n",argv[0]);
        exit(1);
    }

    sscanf(argv[2],"%d",&num);
    if(num > MYTBF_MAX)
    {
        fprintf(stderr,"num should not exceed %d\n",MYTBF_MAX);
        exit(2);
    }

    thd_createjob(0,10,500,argv[1],"/tmp/1");
    thd_createjob(1,50,500,argv[1],"/tmp/2");
    thd_createjob(2,100,500,argv[1],"/tmp/3");

    sleep(200);
    return 0;
}
