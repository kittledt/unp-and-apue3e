
main()
{
     printf("***before fork my pid %d *****\n",getpid());
     fork();
     fork();
     fork();
     printf("***after fork,my pid %d  *****\n",getpid());


}
