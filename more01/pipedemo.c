
/* demo pipe

    proc use pipe , send data to itself.


 */

 #include<stdio.h>
 #include<unistd.h>

 main()
 {

    //create 2 fd,  one for pipe[0],one for pipe[1],
    // fd = 0,1,2,     3,4
    int fd_in, fd_out;
    int array[2];
    char buf[BUFSIZ];
    int len,i;



    if(pipe(array) == -1)
    {
        perror ( "could not make pipe");
        exit(1);

    }

    printf("create pipe , fd is %d and %d\n",array[0],array[1]);

    while(fgets(buf,BUFSIZ,stdin))
    {
        len = strlen(buf);

        if(write(array[1],buf,len) != len)
        {   //send into pipe
            perror("write into pipe");
            break;

        }

        for(i= 0 ; i<len; i++)      //wipe
            buf[i] = 'X';
        len = read(array[0],buf,BUFSIZ);
        if(len == -1)
        {
            perror("reading from pipe");
            break;

        }

        if(write(1,buf,len)!= len)
        {

            perror("writing to stdout");
            break;
        }
    }











 }
