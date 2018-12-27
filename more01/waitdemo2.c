/* show wait()

   get child process status.



*/


#include<stdio.h>
#include<sys/wait.h>

#define DELAY 20
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
    int child_status;
    int high8,low_7,bit_7;

    wait_rv = wait(&child_status);
    printf("done waiting for  %d , will return %d  \n",childpid, wait_rv);

    high8 = child_status >> 8;      /* 1111 1111 0000 0000 */
    low_7 = child_status & 0x7F;    /* 0000 0000 0111 1111*/
    bit_7 = child_status & 0x80;    /* 0000 0000 1000 0000*/
    printf("size of int =   %d   \n",sizeof(int));
    printf("status :exit  = %d ; core dump = %d; sig = %d;\n",high8,bit_7,low_7);

}

