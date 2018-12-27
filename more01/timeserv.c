
/*

    server 6 steps
    1 socket
    2 bind
    3 listen
    4 accept
    5 r/w
    6 close


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
#define PORTNUM 13000 //port
#define oops(m,x) {perror(m); exit(x);}

int main(int argc, int *argv[])
{

    int sock_id,sock_fd;        //socket like file
    struct sockaddr_in saddr;
    struct hostent  *hp;
    char hostname[HOSTLEN];

    FILE *sock_fp;              //use socket as stream ,have buffer

    timer_t thetime;

    char *ctime();

    //1 ask kernal for socket
    sock_id  = socket(PF_INET,SOCK_STREAM,0);
    if (sock_id == -1)
        oops("socket",1);

    //2 bind address to socket, address = host + port
    bzero((void*)&saddr, sizeof(saddr));
    gethostname(hostname,HOSTLEN);
    hp = gethostbyname(hostname);       //get info about host

    bcopy((void*)hp->h_addr,(void*)&saddr.sin_addr,hp->h_length);

    saddr.sin_port = htons(PORTNUM);
    saddr.sin_family = AF_INET;

    if(bind(sock_id,(struct sockaddr *)&saddr,sizeof(saddr)) != 0)
        oops("bind",2);

    //step3 listen
    if(listen(sock_id,1)!= 0)           //queue len is 1
        oops("listen",3);


    //main loop  accept, r/w close
    while(1)
    {
        sock_fd = accept(sock_id, NULL,NULL);       //block current server proc till call come, and return fd.
            printf("get a call!\n");
        if(sock_fd == -1)
            oops("accept",4);

        sock_fp = fdopen(sock_fd,"w");

        if(sock_fp == NULL)
            oops("fdopen",5);

        thetime = time(NULL);

        fprintf(sock_fp,"the time here is ..");
        fprintf(sock_fp,"%s\n",ctime(&thetime));

        fclose(sock_fp);            //release connection


        sleep(2);
        printf("timeserv is running..\n");

    }




}

