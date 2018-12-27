/***************************************************************************
 * logfilec.c: functions for the server of the license server
 * simple logfile server use UNIX domain datagram socket
 * usage: logfilec "message"
 */


#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>


/*
 * Important variables used throughout
 */

#define SOCKET "/tmp/logfilesock"
#define oops(m,x) {perror(m); exit(x);}

int main(int argc, char *argv[])
{

    int sock_id;        //socket like file
    char *msg = argv[1];        //msg content
    struct sockaddr_un saddr;   // address here
    socklen_t addrlen;


    char sockname[] = SOCKET;

    if(argc != 2 )
    {
        fprintf(stderr,"usage: logfilec 'message'\n");
        exit(1);
    }


    sock_id  = socket(PF_UNIX,SOCK_DGRAM,0);        //note PF_UNIX
    if (sock_id == -1)
        oops("socket",2);

    //2 bind address to socket, address = host + port
    saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path,sockname);                //filename is address
    addrlen = strlen(sockname)+ sizeof(saddr.sun_family);


    if(sendto(sock_id,msg,strlen(msg),0 , &saddr, addrlen) == -1)
        oops("sendto",3)

}
