/*
    simulate who command
    author dingtao
    date   13/09/2018

    Ver.2
    add buffer

*/
/*
    utmpopen()
    utmpnext()
    utmpclose()
*/

#include <stdio.h>
#include <utmp.h>       /*read file and get login data*/
#include<unistd.h>   /*file read close*/
#include<fcntl.h>   /* file open*/
#include<time.h>

#define RECORDNUM 16
static  int fd = -1;
static  int rec_num_inbuf = 0;
static  int rec_current = 0;
static int utmpbuf[RECORDNUM * sizeof(struct utmp)];

int utmp_open()
{

    fd= open("/var/run/utmp",O_RDONLY));
    if(fd == -1)
    {
        /*open file failed*/
        printf("open file error\n");
        return -1;
    }
    return 0
}

stuct utmp* utmp_next()
{
    stuct utmp* ptrrec= NULL;
    if(fd == -1)
    {
        /*open file failed*/
        printf("open file error\n");
        return (stuct utmp*)NULL;
    }
    //first check buf, if no record, then reload from file for next 16 or <16 records
    // if utmp_reload return 0, file EOF
    // if rec_current == rec_num_inbuf , buf all used.
    //
    if(rec_current == rec_num_inbuf && utmp_reload()==0)
        return (stuct utmp*)NULL;

    //buf has record, return 1 recored, then more pointer to next one not use.
    ptrrec = (stuct utmp*)  &utmpbuf[rec_current* sizeof(struct utmp)] ;
    rec_current++;
    return ptrrec;



}

/* try read from file for next 16 records
    if last read, maybe <16 records
    return value = actually read.
*/
int utmp_reload()
{
    int size_rd;
    size_rd =  read(fd, utmpbuf, sizeof(struct utmp)* RECORDNUM));
    rec_num_inbuf =  size_rd/ sizeof(struct utmp);   // 16 or if last, <16

    rec_current = 0;
    return rec_num_inbuf;
}

/*close*/
utmp_close()
{

    if(fd != -1)
        close(fd);
}
