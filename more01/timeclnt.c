
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>
#include<netdb.h>

#define oops(m,x) {perror(m); exit(x);}

int main(int argc, int *argv[])
{

    int sock_id,sock_fd;        //socket like file
    struct sockaddr_in servaddr;
    struct hostent  *hp;
    char message[BUFSIZ];       //to receive msg
    int msglen;

    sock_id  = socket(PF_INET,SOCK_STREAM,0);
    if (sock_id == -1)
        oops("socket",1);

    bzero((void*)&servaddr, sizeof(servaddr));

    hp = gethostbyname(argv[1]);

    if (hp == NULL)
        oops(argv[1],2);

    bcopy((void*)hp->h_addr,(void*)&servaddr.sin_addr,hp->h_length);

    servaddr.sin_port = htons(atoi(argv[2]));               //host to netwrik long
    servaddr.sin_family = AF_INET;

    if(connect(sock_id,(struct sockaddr*)&servaddr,sizeof(servaddr)) != 0)
        oops("connect",3);

    msglen = read(sock_id,message,BUFSIZ);      //read stuff
    if(msglen == -1)
        oops("read",4);
    if(write(1,message, msglen)!= msglen)
        oops("write", 5);
    close(sock_id);




}
