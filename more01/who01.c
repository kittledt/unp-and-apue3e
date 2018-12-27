/*
    simulate who command
    author dingtao
    date   12/09/2018




*/

#include <stdio.h>
#include <utmp.h>       /*read file and get login data*/
#include<unistd.h>   /*file read close*/
#include<fcntl.h>   /* file open*/
#include<time.h>

showinfo(struct utmp *utmpbuf);
showinfo2(struct utmp *utmpbuf);

int main()
{
    int fd;
    int size_rd;
    struct utmp record;

    if(  fd= open("/var/run/utmp",O_RDONLY))
    {
        while(  size_rd =  read(fd, &record,sizeof(struct utmp)))
        {
            /*read file for struct utmp size */
            if(size_rd == sizeof(struct utmp))
            {
                /*printf("read 1 record\n");*/
                /*showinfo(&record);*/
                showinfo2(&record);
            }
            else if (size_rd == -1)
            {
                /*read fail*/
                printf("read fail\n");
                exit(1);
            }


        }
    }
    else if(fd == -1)
    {
        /*open file failed*/
        printf("open file error\n");
        exit(1);
    }
    else
        exit(1);

    close(fd);

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

