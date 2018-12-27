/**************
 file_lock_ts.c - read the current date/time from a file
 *     usage: file_ts filename
 *    action: writes the current time/date to filename
 *     note: uses fcntl()-based locking
 */


#include<stdio.h>
#include<sys/file.h>

#include<fcntl.h>
#include<time.h>

#define  oops(m,x)  { perror(m); exit(x);}

main(int ac, char *av[])
{

	int	fd;
	time_t  now;
	char    *message;

	if ( ac != 2 ){

		fprintf(stderr,"usage: file_ts filename\n");
		exit(1);

    }
	if ( (fd = open(av[1],O_CREAT|O_TRUNC|O_WRONLY,0644)) == -1 )
		oops(av[1],2);

	while(1)
	{

		time(&now);
		message = ctime(&now);		/* compute time	     */

		lock_operation(fd, F_WRLCK);	/* lock for writing  */

		if ( lseek(fd, 0L, SEEK_SET) == -1 )        /* Seek from beginning of file.  */
			oops("lseek",3);
		if ( write(fd, message, strlen(message)) == -1 )
			oops("write", 4);

		lock_operation(fd, F_UNLCK);	/* unlock file	     */
		sleep(1);			            /* wait for new time */

}

}

lock_operation(int fd, int op)
{

	struct flock lockinfo;

	lockinfo.l_whence = SEEK_SET;                       //start of file
	lockinfo.l_start = 0;                               //begin positon
    lockinfo.l_len = 0;                                 //until EOF
	lockinfo.l_pid = getpid();                          //current process
	lockinfo.l_type = op;                               // F_WRLCK to lock ,and F_UNLCK to unlock

	if ( fcntl(fd, F_SETLKW, &lockinfo) == -1 )         //Set record locking info (blocking)
		oops("lock operation", 6);

}
