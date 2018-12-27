/***************************************************************************
 * logfiled.c: functions for the client of the license server
 * simple logfile server use UNIX domain datagram socket
 * usage: logfiled>>logfilename
 */


#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>

/*
 * Important variables used throughout
 */
#define MSGLEN	128	                /* Size of our datagrams */
#define SOCKNAME "/tmp/logfilesock"
#define oops(m,x) {perror(m); exit(x);}

int main(int argc, char *argv[])
{

    int sock_id;        //socket like file

    struct sockaddr_un saddr;   // address here
    socklen_t addrlen;
    char msg[MSGLEN];
    int l;
    char sockname[] = SOCKNAME;
    time_t now;
    int msgnum = 0;
    char *timestr;


    unlink (sockname);                  /*delete  socket, to avoid object occpuied by last run , and bind error */
    sock_id  = socket(PF_UNIX,SOCK_DGRAM,0);        //note PF_UNIX
    if (sock_id == -1)
        oops("socket",2);

    //2 bind address to socket, address = host + port
    saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path,sockname);                //filename is address
    addrlen = strlen(sockname)+ sizeof(saddr.sun_family);

    if(bind(sock_id,(struct sockaddr *)&saddr,addrlen) == -1)
        oops("bind",3);
    while(1)
    {
        l = read(sock_id,msg,MSGLEN);
        printf("receive %d\n", l);
        msg[l] = '\0';
        time(&now);
        timestr = ctime(&now);
        timestr[strlen(timestr)- 1] ='\0';          //chop newline

        printf("[%5d] %s %s\n", msgnum++,timestr,msg);
        fflush(stdout);

    }

}
