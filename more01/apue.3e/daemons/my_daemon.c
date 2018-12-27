/****

	main() for call daemonize()

	
*/

#include "apue.h"

void daemonize(const char *cmd);

void
main()
{
	printf("will daemonize current process,use ps -axj check\n");
	daemonize("my_daemon");
	
		
}

