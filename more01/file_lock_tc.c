/*******************
        file_lock_tc.c - read the current date/time from a file
 *     usage: file_tc filename
 *      uses: fcntl()-based locking
 */

#include<stdio.h>
#include<sys/file.h>

#include<fcntl.h>

#define  oops(m,x)  { perror(m); exit(x);}
#define  BUFLEN 10

main(int ac, char *av[])
{

	int	fd, nread;
	char	buf[BUFLEN];

	if ( ac != 2 ){

		fprintf(stderr,"usage: file_tc filename\n");
		exit(1);

    }

	if ( (fd= open(av[1],O_RDONLY)) == -1 )
		oops(av[1],3);

	lock_operation(fd, F_RDLCK);            //read lock

    while( (nread = read(fd, buf, BUFLEN)) > 0 )
		write(1, buf, nread );              //output to stdout

	lock_operation(fd, F_UNLCK);

	close(fd);

}

lock_operation(int fd, int op)
{

	struct flock lockinfo;

	lockinfo.l_whence = SEEK_SET;
	lockinfo.l_start = 0;
	lockinfo.l_len = 0;
	lockinfo.l_pid = getpid();
	lockinfo.l_type = op;

	if ( fcntl(fd, F_SETLKW, &lockinfo) == -1 )     /* Set record locking info (blocking).	*/
		oops("lock operation", 6);

}
