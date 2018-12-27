
#include<unistd.h>
#include<stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

static  int j = 0;
void sig_chld(int no)
{
        int i;
        while ((i = waitpid(-1, NULL, WNOHANG)) > 0)
        //while ((i = wait( NULL)) > 0)
        {
                printf("a child terminited.pid = %d\n",i);
        }
        printf("%d times, return value is : %d \n",j,i);
        j++;
        return;
}

int main()
{
        signal(SIGCHLD, sig_chld);
        int i;
        int iarray[5];      //store child pid .
        pid_t pid;
        for ( i=0; i<5; i++)
        {
                if((pid =fork()) == 0)      // in child proc
                {
                        sleep(2);           //child sleep 2 , then return
                        return ;
                }
                else if (pid > 0 )
                {
                    iarray[i] = pid;        //store child pid
                    printf("pid is : %d \n",pid);
                }

        }


        while(1)                        // parent running and wait.
           {
                     sleep(10);
             }

        return 0;
}
