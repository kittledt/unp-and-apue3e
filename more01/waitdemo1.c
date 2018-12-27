/* show wait()

    not get child process status.

*/


#include<stdio.h>

#define DELAY 2
main()
{

    int newpid;
    void child_code(), parent_code();
    printf("before: my pid is %d \n",getpid());

    if((newpid = fork()) == -1)
        perror("fork error");
    else if (newpid == 0)
        child_code(DELAY);
    else
        parent_code(newpid);

}

void child_code(int delay)
{
    printf("child here pid is %d , will sleep %d seconds \n",getpid(),delay);
    sleep(delay);
    printf("child done. about to exit \n");
    exit(17);

}

void parent_code(int childpid)
{


    int wait_rv ;// return value from wait()
    wait_rv = wait(NULL);
    printf("done waiting for  %d , will return %d  \n",childpid, wait_rv);


}
