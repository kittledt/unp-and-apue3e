/*
    socklib.c

    int make_server_socket(int portnum)  return a server socket,  or -1 if error
     *********** socket(),bind(),listen();

    int make_server_socket_q(portnum, backlog)          queue len is 1

    int connect_to_server(char *hostname, int portnum)   return a client socket or -1 if error
    *************socket(),connect()

*/

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>
#include<netdb.h>
#include<time.h>
#include<strings.h>

#define HOSTLEN 256
//#define PORTNUM 13000 //port
#define BACKLOG 1
#define oops(m,x) {perror(m); return(x);}


int make_server_socket_q(int portnum, int backlog);
int make_server_socket(int portnum)
{
    return make_server_socket_q(portnum,BACKLOG);

}

int make_server_socket_q(int portnum, int backlog)
{

    int sock_id;        //socket like file
    struct sockaddr_in saddr;
    struct hostent  *hp;
    char hostname[HOSTLEN];


    //1 ask kernal for socket
    sock_id  = socket(PF_INET,SOCK_STREAM,0);
    if (sock_id == -1)
        oops("socket",-1);

    //2 bind address to socket, address = host + port
    bzero((void*)&saddr, sizeof(saddr));            //clear struct
    gethostname(hostname,HOSTLEN);                  //where am i?
    hp = gethostbyname(hostname);                   //get info about host

    bcopy((void*)hp->h_addr,(void*)&saddr.sin_addr,hp->h_length);       //fill in host part

    saddr.sin_port = htons(portnum);                                    //fill in port part
    saddr.sin_family = AF_INET;

    if(bind(sock_id,(struct sockaddr *)&saddr,sizeof(saddr)) != 0)
        oops("bind",-1);

    //step3 listen
    if(listen(sock_id,backlog)!= 0)           //queue len is 1
        oops("listen",-1);

    return sock_id;

}


int connect_to_server(char *hostname, int portnum)
{
    int sock_id;        //socket like file
    struct sockaddr_in servaddr;
    struct hostent  *hp;


    sock_id  = socket(AF_INET,SOCK_STREAM,0);
    if (sock_id == -1)
        oops("socket",-1);

    bzero((void*)&servaddr, sizeof(servaddr));

    hp = gethostbyname(hostname);

    if (hp == NULL)
        oops(hostname,-1);

    bcopy((void*)hp->h_addr,(void*)&servaddr.sin_addr,hp->h_length);

    servaddr.sin_port = htons(portnum);               //host to netwrik long
    servaddr.sin_family = AF_INET;

    if(connect(sock_id,(struct sockaddr*)&servaddr,sizeof(servaddr)) != 0)
        oops("connect",-1);


    return sock_id;

}
