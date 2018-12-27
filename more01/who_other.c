

//who.c new version with buffer
#include <stdio.h>
#include <utmp.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>

#define NUM_BUF	 16
#define NULLUT	 ((struct utmp*)NULL)
#define UT_SIZE	 (sizeof(struct utmp))
#define UT_FILE  "/var/run/utmp"

static char buf[UT_SIZE * NUM_BUF];
static int cur_rec;					/* already read  in buffer*/
static int num_recs; 				/* num stored in buffer */
static int fd_utmp = -1;

int utmp_open()
{
	fd_utmp = open(UT_FILE,O_RDONLY);
	cur_rec = 0;
	num_recs = 0;
	return fd_utmp;

}

struct utmp* utmp_next()
{
	struct utmp* recp;
	if(fd_utmp == -1)
	{
		perror("open error:");
		return NULLUT;
	}
	if(cur_rec == num_recs && (utmp_reload() == 0)) /* wether equal,then reload */
		return NULLUT;
	recp  = (struct utmp*)&buf[cur_rec * UT_SIZE];
	printf("rec_current* sizeof(struct utmp) = %d \n",cur_rec * UT_SIZE);
	cur_rec++;
	return recp;
}

int utmp_reload()
{
	int ret;
	ret = read(fd_utmp,buf,UT_SIZE * NUM_BUF);
	if(ret == -1)
	{
		perror("read error:");
		exit(0);
	}
	cur_rec = 0;
	num_recs = ret / UT_SIZE;
	return num_recs;
}

int utmp_close()
{
		if(close(fd_utmp) == -1)
			perror("close");
}

void show_time(time_t t)
{
	char* p_time;
	p_time = ctime(&t);
	printf("%18.18s", p_time + 4);
}

void show_info(struct utmp* utmp_buffer)
{


	if(utmp_buffer->ut_type == USER_PROCESS)
	{
		printf("%s",utmp_buffer->ut_user);
		printf("\t");
		printf("%s",utmp_buffer->ut_line);
		printf("\t");
		show_time(utmp_buffer->ut_tv.tv_sec);


		printf("(%s)",utmp_buffer->ut_host);


		printf("\n");
	}
}



int main()
{
	struct utmp* ut_fp;
	if(utmp_open() == -1)
	{
		perror("open error:");
		exit(-1);
	}
	while((ut_fp = utmp_next()) != NULLUT)
		show_info(ut_fp);
	utmp_close();
}
