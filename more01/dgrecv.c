/*********************************************
    datagram receiver
    usage: dgrecv portnum
    build: build: cc dgrecv.c dgram.c -o dgrecv
    action: wait message come up in specified port


*/
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#define oops(m,x) {perror(m); exit(x);}

int make_dgram_server_socket(int);
int get_internet_address(char*, int, int*, struct sockaddr_in*);
void say_who_called(struct sockaddr_in* addrp)
{
    char host[BUFSIZ];
    int port;
    get_internet_address(host,BUFSIZ,&port,addrp);
    printf("from %s :%d\n",host, port);

}

int main(int argc, char *argv[])
{

    int sock_id;        //socket like file


    int port;
    char buf[BUFSIZ];
    size_t msglen;
    struct sockaddr_in saddr;   //put sender's address here
    socklen_t saddrlen;         //its length

    if(argc == 1 || (port = atoi(argv[1])) <= 0)
    {
        fprintf(stderr,"usage: dgrecv portnum\n");
        exit(1);
    }

    if((sock_id = make_dgram_server_socket(port))  == -1)
        oops("can not make socket",2);

    //receive message
    saddrlen = sizeof(&saddr);
    while((msglen = recvfrom(sock_id,buf,BUFSIZ,0,&saddr,&saddrlen))> 0)
    {
        printf("dgrecv: message len:%d\n",msglen);
        buf[msglen] = '\0';
        printf("dgrecv: got a message:%s\n",buf);
        say_who_called(&saddr);                 //saddr is sender's addr

    }
}

