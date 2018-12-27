
/*********************************************
    datagram sender
    usage: dgsend hostname portnum "message"
    build: build: cc dgsend.c dgram.c -o dgsend
    action: send message


*/
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>
#define oops(m,x) {perror(m); exit(x);}

int make_dgram_client_socket();
int make_internet_address(char* hostname, int port,  struct sockaddr_in* addrp);

int main(int argc, char *argv[])
{

    int sock_id;        //socket like file
    char *msg;
    struct sockaddr_in saddr;   //put sender's address here
    int nchars;         //len of send msg

    if(argc != 4)
    {
        fprintf(stderr,"dgsend hostname portnum message\n");
        exit(1);
    }

    msg = argv[3] ;  // msg buf has exsit

    if((sock_id = make_dgram_client_socket()) == -1)
        oops("can not make socket", 2);

    make_internet_address(argv[1],atoi(argv[2]),&saddr);

    if((nchars = sendto(sock_id, msg, strlen(msg),0,&saddr,sizeof(saddr)) )== -1)
        oops("sendto failed",3)
    else
        printf("send msg len = %d\n",nchars);
    return 0;
}
