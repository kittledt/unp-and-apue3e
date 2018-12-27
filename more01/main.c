#include <stdio.h>
#include <stdlib.h>
#define LINELEN 512
#define PAGELEN 24
void do_more(FILE* f);
int see_more(FILE* cmd);
int main(int argc, char** argv)
{
    FILE* fp;
    if(argc==1)
        do_more(stdin);
    else
        while(--argc)
        {
            if((fp=fopen(*(++argv),"r"))!=NULL)
            {
                do_more(fp);
                fclose(fp);
            }
            else
                exit(1);
        }
    return 0;
}
void do_more(FILE* fp)
{
    char line[LINELEN];              /* char array, size 512*/
    int num_of_lines=0;
    int reply;
    FILE* tty_file;
    if((tty_file=fopen("/dev/tty","r"))==NULL)        /*keyboard and screen input, output <>*/
        exit(1);

    while(fgets(line,LINELEN,fp))        /* file stream*/
    {
        if(num_of_lines==PAGELEN)       /*full screen?  to max 24 lines*/
        {
            reply=see_more(tty_file);   /* ask user?  1 or 24 lines or 0 =quit*/
            if(reply==0)
                break;
            num_of_lines-=reply;        /* 24-1 = 23, or 24-24 = 0   means add buf */
        }
        if(fputs(line,stdout)==EOF)      /* 1 line, output to stdout, */
            exit(1);
        num_of_lines++;             /*next line*/

    }
}
int see_more(FILE* cmd)
{
    int c;
    /*printf("more?");*/
    printf("\033[7m more? \033[m");           /*reverse color on vt100*/
    while((c=fgetc(cmd))!=EOF)
    {
        if(c=='q')
            return 0;
        if(c==' ')
            return PAGELEN;     /*space -> next page, 24 line*/
        if(c=='\n')
            return 1;           /*Enter  -> next line, 1 line*/
    }
    return 0;
}
