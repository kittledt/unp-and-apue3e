
/*
    web server, handle GET request


    build: cc webserv.c socklib.c -o webserv
    server run:./webserv 12345
    client test run :
    1.list dir ---- curl ecallspirent01:12345
    2.exec cmd ---- curl ecallspirent01:12345/hello.cgi
    3.cat file ---- ?


*/
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>

//skip over all request until a CRNL is seen
read_til_crnl(FILE *fp)
{
    char buf[BUFSIZ];
    while(fgets(buf,BUFSIZ,fp)!= NULL &&  strcmp(buf,"\r\n")!= 0 );

}

cannot_do(int fd)
{
    FILE *fp = fdopen(fd,"w");

    fprintf(fp,"HTTP/1.0 501 Not Implemented\r\n");
    fprintf(fp,"Content-type:text/palain\r\n");
    fprintf(fp,"\r\n");

    fprintf(fp,"That command is not yet implemented\r\n");
    fclose(fp);

}

do_404(char *item,int fd)
{
    FILE *fp = fdopen(fd,"w");
    fprintf(fp,"HTTP/1.0 404 Not Found\r\n");
    fprintf(fp,"Content-type:text/plain\r\n");
    fprintf(fp,"\r\n");

    fprintf(fp,"The item you requested:%s\r\n is not found\r\n",item);
    fclose(fp);
}


isadir(char *f)
{
    struct stat info;
    return (stat(f,&info) != -1 && S_ISDIR(info.st_mode));
}

not_exist(char *f)
{
    struct stat info;
    return (stat(f,&info) == -1);
}



do_ls(char *dir,int fd)
{
    FILE *fp;
    fp = fdopen(fd,"w");
    header(fp,"text/plain");
    fprintf(fp,"\r\n");
    fflush(fp);

    dup2(fd,1);
    dup2(fd,2);
    close(fd);
    execlp("ls","ls","-l",dir,NULL);//exec ls -l
    perror(dir);
    exit(1);
}

char *file_type(char *f)
{
    char *cp;
    if((cp = strrchr(f,'.')) != NULL)
            return cp +1;
    return "";
}

ends_in_cgi(char *f)
{
    return (strcmp(file_type(f),"cgi") == 0);
}

do_exec(char *prog ,int fd)
{
    FILE *fp;

    fp = fdopen(fd,"w");
    header(fp,NULL);
    fflush(fp);
    dup2(fd,1);
    dup2(fd,2);
    close(fd);
    execl(prog,prog,NULL);
    perror(prog);
}

//send back contents after a header
do_cat(char *f,int fd)
{
    char *extension = file_type(f);//judge file type
    char *content = "text/plain";
    FILE *fpsock,*fpfile;
    int c;

    if(strcmp(extension,"html") == 0)
            content = "text/html";
    else if(strcmp(extension,"gif") == 0)
            content = "image/gif";
    else if(strcmp(extension,"jpg") == 0)
            content = "image/jpeg";
    else if(strcmp(extension,"jpeg") == 0)
            content = "image/jpeg";

    fpsock = fdopen(fd,"w");
    fpfile = fopen(f,"r");
    if(fpsock != NULL&&fpfile != NULL)
    {
        header(fpsock,content);
        fprintf(fpsock,"\r\n");
        while((c = getc(fpfile)) != EOF)// response content
                putc(c,fpsock);
        fclose(fpfile);
        fclose(fpsock);

    }
    exit(0);
}


//
// rq is HTTP command: GET /foo/bar.html HTTP/1.0
process_rq(char *rq, int fd)
{
    char cmd[BUFSIZ], arg[BUFSIZ];

    if(fork() != 0)   // creat new process
            return;
    strcpy(arg,"./");

    if(sscanf(rq,"%s%s",cmd,arg+2) != 2)//parse cmd and arg
            return;
    if(strcmp(cmd,"GET") != 0)            // acoording to arg return
            cannot_do(fd);
    else if(not_exist(arg))
            do_404(arg,fd);
    else if(isadir(arg))                    //ls dir
            do_ls(arg,fd);
    else if(ends_in_cgi(arg))               //run .cgi file
            do_exec(arg,fd);
    else
            do_cat(arg,fd);                 //check file
}

header(FILE *fp,char * content_type)
{
    fprintf(fp,"HTTP/1.0 200 OK\r\n");
    if(content_type)
            fprintf(fp,"Content-type:%s\r\n",content_type);

}

int main(int argc, char *argv[])
{
    int sock_id, fd;
    FILE *fpin;

    char request[BUFSIZ];
    if(argc == 1)
    {
        fprintf(stderr,"usage: ws portnum\n");
        exit(1);
    }

    sock_id = make_server_socket(atoi(argv[1]));
    if(sock_id == -1)
        exit(2);
    while(1)
    {
        fd = accept(sock_id,NULL,NULL);         //block and wait connect
        fpin = fdopen(fd,"r");

        //read request
        fgets(request,BUFSIZ,fpin);
        printf("got a call: request = %s",request);
        read_til_crnl(fpin);

        process_rq(request,fd);
        fclose(fpin);


    }

}
