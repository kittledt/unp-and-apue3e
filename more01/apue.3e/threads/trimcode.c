
/*****************
	to delete number int ther front of lines

	example:
	34
	35     puts("End!");

	remove 34,35.
*/

#include "apue.h"

int
main(int argc, char* argv[])
{
	char	buf[MAXLINE];
	int num;
    if(argc != 2)
    {    printf("usage:trimcode 3 < src.c > dst.c\n");
         exit(1);
    }

    sscanf(argv[1],"%d",&num);
    printf("num = %d\n",num);
    fflush(stdout);

	while (fgets(buf, MAXLINE, stdin) != NULL)
    {
        if (fputs(buf+num, stdout) == EOF)
            err_sys("output error");
    }
	if (ferror(stdin))
		err_sys("input error");

	exit(0);
}
