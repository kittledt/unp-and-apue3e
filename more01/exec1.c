
#include<stdio.h>
#include<unistd.h>
main()
{
    char * arglist[3];
    arglist[0] = "ls";
    arglist[1] = "-l";
    arglist[2] = 0;         //last one should be null
    printf("***about exec ls -l *****\n");
    execvp("ls",arglist);
    printf("***ls -l is done  *****\n");



}
