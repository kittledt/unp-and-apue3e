#include "apue.h"

int
main(void)
{
	if (chdir("/tmp") < 0)
		err_sys("chdir failed");
	printf("chdir to /tmp succeeded\n");
	printf("current pid = %d\n",getpid());
	printf("use cmd ps check process\n");
	sleep(20);
	exit(0);
}
