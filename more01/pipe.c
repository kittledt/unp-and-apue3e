

/* demo pipe

     with 2 args
    like pipe  who sort  =  who | sort

    who's output to sort's input.


 */

#include<stdio.h>
#include<unistd.h>


#define oops(m,x) {perror(m); exit(x);}


main(int argc, int *argv[])
{

    int pipefd[2];
    int pid;

    if(argc != 3)
        oops("usage: pipe cmd1 cmd2\n" ,1 );

    if(pipe(pipefd)== -1 )
        oops("could not make pipe",2);

    if ((pid = fork()) == -1)
         oops("could not fork",3);

    if (pid > 0)
    {
        //in parent, parent proc will exec argv[2]
        close(pipefd[1]);       //perent not need wirte

        if(dup2(pipefd[0],0) == -1)
            oops("could not redirect stdin of parent",3);

        close(pipefd[0]);
        execlp(argv[2],argv[2],NULL);
        oops(argv[2],4);        // exec error then , run this
    }

    //in child, child proc will exec argv[1]
    close(pipefd[0])  ;         //child not need read
    if(dup2(pipefd[1],1) == -1)
            oops("could not redirect stdout of child",5);

        close(pipefd[1]);
        execlp(argv[1],argv[1],NULL);
        oops(argv[1],6);
}
