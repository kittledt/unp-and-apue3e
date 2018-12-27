/*
    simulate who command
    author dingtao
    date   13/09/2018

    Ver.2
    add buffer

*/

#include <stdio.h>
#include <utmp.h>       /*read file and get login data*/
#include<unistd.h>   /*file read close*/
#include<fcntl.h>   /* file open*/
#include<time.h>
//#include<utmplib.h>
#define RECORDNUM 16
#define  UT_SIZE	 (sizeof(struct utmp))
static  int fd = -1;
static  int rec_num_inbuf = 0;
static  int rec_current = 0;
static  char utmpbuf[RECORDNUM * UT_SIZE];       /* bug, use int  not char, so     384*4 =1536  */

showinfo(struct utmp *utmpbuf);
showinfo2(struct utmp *utmpbuf);


int utmp_open()
{

    fd= open("/var/run/utmp",O_RDONLY);
    if(fd == -1)
    {
        /*open file failed*/
        printf("open file error\n");
        return -1;
    }
    return 0;
}

struct utmp* utmp_next()
{
    struct utmp* ptrrec= NULL;
    if(fd == -1)
    {
        /*open file failed*/
        printf("open file error\n");
        return (struct utmp*)NULL;
    }
    //first check buf, if no record, then reload from file for next 16 or <16 records
    // if utmp_reload return 0, file EOF
    // if rec_current == rec_num_inbuf , buf all used.
    //
    if(rec_current == rec_num_inbuf && utmp_reload()==0)
        return (struct utmp*)NULL;

    //buf has record, return 1 recored, then more pointer to next one not use.
    ptrrec = (struct utmp*)  &utmpbuf[rec_current* UT_SIZE] ;
    //printf("rec_current = %d \n",rec_current);
    //printf("rec_current* sizeof(struct utmp) = %d \n",rec_current* UT_SIZE);
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
    size_rd =  read(fd, utmpbuf, UT_SIZE* RECORDNUM);
    rec_num_inbuf =  size_rd/ UT_SIZE;   // 16 or if last, <16
    //printf("rec_num_inbuf = %d \n",rec_num_inbuf);
    rec_current = 0;
    return rec_num_inbuf;
}

/*close*/
utmp_close()
{

    if(fd != -1)
        close(fd);
}

int main()
{
    int fd;
    int size_rd;
    struct utmp record;
    struct utmp* ptr;
    // open file with utmplib function
    if(utmp_open() == -1)
        exit(1);

    /* isolate file read operation with utmplib function */
    while( (ptr = utmp_next()) != (struct utmp*)NULL)         //get 1 utmp struct and show untill EOF
    {
        showinfo2(ptr);
    }

    utmp_close();

    return 0;
}

/*
    V1.0
*/
showinfo(struct utmp *utmpbuf)
{
    printf("%-8.8s",utmpbuf->ut_name);
    printf(" ");
    printf("%-8.8s",utmpbuf->ut_line);
    printf(" ");
    printf("%10ld",utmpbuf->ut_time);
    printf(" ");
    printf("(%s)",utmpbuf->ut_host);
    printf("\n");


}

/*
    V2.0
        can trim off empty login record(only display active user)
        can display readable time
*/
void show_time(time_t t)
{
	char* p_time;
	p_time = ctime(&t);
	printf("%18.18s", p_time + 4);
}
showinfo2(struct utmp *utmpbuf)
{
    if(utmpbuf->ut_type != USER_PROCESS)    /* not a active user*/
        return;
    printf("%-8.8s",utmpbuf->ut_name);
    printf(" ");
    printf("%-8.8s",utmpbuf->ut_line);
    printf(" ");
    //printf("%18.18s",ctime(&(utmpbuf->ut_tv.tv_sec)));
    show_time(utmpbuf->ut_tv.tv_sec);
    printf(" ");
    printf("(%s)",utmpbuf->ut_host);
    printf("\n");


}


