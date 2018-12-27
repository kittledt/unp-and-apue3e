

/* demo pipe

    parent and child share pipe


 */

 #include<stdio.h>
 #include<unistd.h>


#define CHILD_MESS "child,i want a cookie\n"

#define PAR_MESS "parent, testing...\n"
 main()
 {
    int pipefd[2];
    char buf[BUFSIZ];
    int len;
    int read_len;



    if(pipe(pipefd) == -1)
    {
        perror ( "could not make pipe");
        exit(1);

    }

    switch(fork())
    {
        case -1:
            perror ( "could not fork");
            exit(2);

        case 0:
            // in child
            len = strlen( CHILD_MESS );
            while(1)
            {
                if(write(pipefd[1],CHILD_MESS,len)!= len)
                {
                    perror ( "could not write child");
                    exit(3);
                }
                sleep(5);
            }
        default:
            //in parent
            len = strlen( PAR_MESS );
            while(1)
            {
                if(write(pipefd[1],PAR_MESS,len)!= len)
                {
                    perror ( "could not write parent");
                    exit(4);
                }
                sleep(1);
                read_len = read(pipefd[0],buf,BUFSIZ);
                if(read_len <= 0)
                {
                    break;
                }
                write(1, buf, read_len);
            }

    }

 }
