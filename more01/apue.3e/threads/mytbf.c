
 /* mytbf.c */
 /******************
    multi thread token .

    1024 tocken buckets, can support max 1024 jobs.

    add 1 new job, will get 1 bucket from array .

    for every token bucket, can set token generate speed  and max limitation
        int cps; //speed,  addition of token every time
        int burst; //  tocken max limitation


 */
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <errno.h>
 #include <pthread.h>
 #include <string.h>

 #include "mytbf.h"

 #define BUFSIZE     1024

 /* Token Bucket*/
 struct mytbf_st
 {
     int cps; //speed,  addition of token every time
     int burst; //  tocken max limitation
     int token; // number of tokens in bucket
     int pos; // current bucket reletive job's index in   job[] array
     pthread_mutex_t mut; // mutex to protect this token in racing conditon
 };


struct  arg_st{
        int   icps;
        int   iburst;
        int   ireadfd;
        int   iwritefd;

    };

 /* all 1024 buckets*/
 static struct mytbf_st *job[MYTBF_MAX];
 /* protect 1024 bucket[] array in racing conditon */
 static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;
 /* add tocken thread ID */
 static pthread_t tid;
 /* init tocken add thread */
 static pthread_once_t init_once = PTHREAD_ONCE_INIT;       //pthread_once_t


 static struct arg_st  myarg_st[MYTBF_MAX] ;

 /*  tocken add thread -- func handler */
 static void *thr_alrm(void *p)
 {
     int i;

     while(1)
     {
         pthread_mutex_lock(&mut_job);      //lock all 1024 bucket[] array
         // through all 1024 bucket
         for(i = 0 ; i < MYTBF_MAX; i++)
         {
             // add tocken to every available bucket, only bucket has its job is available.
             if(job[i] != NULL)
             {
                printf("add job[%d] token + %d\n",i,job[i]->cps);
                 pthread_mutex_lock(&job[i]->mut);  //lock 1 bucket
                 job[i]->token += job[i]->cps;
                 // tocken can't exceed max limitation.
                 if(job[i]->token > job[i]->burst)
                     job[i]->token = job[i]->burst;
                 pthread_mutex_unlock(&job[i]->mut);    //unlock 1 bucket
             }
         }
         pthread_mutex_unlock(&mut_job);        //unlock all 1024 bucket[] array

         //wait 1 second for next turn .
         sleep(1);
     }

     pthread_exit(NULL);
 }

/******
    when process exit,  clear:
    1, request cancel  thread- add token
    2, wait thread to complete
*/
 static void module_unload(void)
 {
     int i;

     pthread_cancel(tid);
     pthread_join(tid,NULL);

     pthread_mutex_lock(&mut_job);
     for(i = 0 ; i < MYTBF_MAX ; i++)
     {
         if(job[i] != NULL)
         {
             // destory mutex in bucket
             pthread_mutex_destroy(&job[i]->mut);
             free(job[i]);
         }
     }

     pthread_mutex_unlock(&mut_job);

     pthread_mutex_destroy(&mut_job);

 }

/***
    only called once.
    1, new thread for add token
    2, atexit()  register clean func

*/
 static void module_load(void)
 {
     int err;

     err = pthread_create(&tid,NULL,thr_alrm,NULL);     //new thread for add tocken to every available bucket
     if(err)
     {
         fprintf(stderr,"pthread_create():%s\n",strerror(err));
         exit(1);
     }
     printf("thread--add token running\n");
     atexit(module_unload);
 }

 /**********
     handle 1 token bucket and all 1024 bucket[] array all need atom operation.
     need lock first.

     but function has jump point. like return, exit, continue; break; goto; longjmp(3) or call another function;
     so unlock first before jump.

  */
 static int get_free_pos_unlocked(void)
 {
     int i;

     for(i = 0 ; i < MYTBF_MAX; i++)
         if(job[i] == NULL)
             return i;
     return -1;
 }


/************
for every 1 token bucket, can set token generate speed  and max limitation
        int cps; //speed,  addition of token every time
        int burst; //  tocken max limitation

        1, ensure  module_load() only called once.
        2, malloc memory struct for bucket
        3, add to job array.

        return  pointer to 1 bucket struct
*/
 mytbf_t *mytbf_init(int cps,int burst)
 {
     struct mytbf_st *me;
     int pos;

    /******
     generate new thread ,but only run thread handler once.
     module_load() only called once.   mytbf_init() first called, it will be called. next times, not called.

    */
     pthread_once(&init_once,module_load);
     me = malloc(sizeof(*me));      //generate 1 token bucket
     if(me == NULL)
         return NULL;

     me->cps = cps;
     me->burst = burst;
     me->token = 0;
     pthread_mutex_init(&me->mut,NULL);

     pthread_mutex_lock(&mut_job);  //lock 1024 bucket[] array

     pos = get_free_pos_unlocked();
     if(pos < 0)
     {
        // error handle , all  1024 buckets in array  already used
         // jump with lock , need unlock first, then jump,   jump point--> return.
         pthread_mutex_unlock(&mut_job);
         free(me);
         return NULL;
     }

     me->pos = pos;

     job[pos] = me;

     pthread_mutex_unlock(&mut_job);

     return me;     //return pointer to new 1 gererate bucket
 }

 static inline int min(int a,int b)
 {
     return (a < b) ? a : b;
 }

 int mytbf_fetchtoken(mytbf_t *ptr,int size)
 {
     int n;
     struct mytbf_st *me = ptr;         //point to a token bucket

     if(size < 0)
         return -EINVAL;

     pthread_mutex_lock(&me->mut);      //lock 1 tocken bucket
     // tocken not enough , wait add thread
     while(me->token <= 0)
     {
         // unlock first ,  give schedluer let other run first.
         pthread_mutex_unlock(&me->mut);
         sched_yield();                 //like sleep very short time. then get the lock .
         pthread_mutex_lock(&me->mut);
     }

     n = min(me->token,size);

     me->token -= n;

     pthread_mutex_unlock(&me->mut);

     return n;
 }

 /* return token ,not waste */
 int mytbf_returntoken(mytbf_t *ptr,int size)
 {
     struct mytbf_st *me = ptr;

     //
     if(size < 0)
             return -EINVAL;

     pthread_mutex_lock(&me->mut);

     me->token += size;
     if(me->token > me->burst)
         me->token = me->burst;

     pthread_mutex_unlock(&me->mut);

     return size;
 }

/**
 destroy 1 bucket ,
    1, delete in job array
    2, destory the bucket's mutex
    3 , free the struct memory
*/

 void mytbf_destroy(mytbf_t *ptr)
 {
     struct mytbf_st *me = ptr;

     pthread_mutex_lock(&mut_job);
     job[me->pos] = NULL;
     pthread_mutex_unlock(&mut_job);

     pthread_mutex_destroy(&me->mut);
     free(ptr);
 }


void add_job(int cps,int burst,int readfd,const char *pfpath)
{
    int writefd;
    struct mytbf_st *me;

    me= mytbf_init(cps,burst);
    write_to_file(readfd,writefd,(void *)me);
}


/**************

    void *p : pointer to token bucket struct

*/

void write_to_file(int readfd,int writefd,void *p)
{
    struct mytbf_st *me;
    int len,ret,pos,size;
    char buf[BUFSIZE];
    me = p;
    while(1)
    {
        size = mytbf_fetchtoken(me,BUFSIZE);        //get token, if tocken value < BUFSIZE(fs block size )
        printf("fetchtoken = %d\n", size);
        while((len = read(readfd,buf,size)) < 0)
        {
                if(errno = EINTR)
                    continue;
                perror("read()");
                break;
        }

        //src file read EOF , jump while
        if (len == 0)
            break;

        mytbf_returntoken(me,size-len);     //token > read bytes. return token.

        //like apue, writen()
        pos = 0;
        while(len > 0)
        {
            ret = write(writefd,buf+pos,len);
            if(ret < 0)
            {
                if(errno == EINTR)
                        continue;
                perror("write()");
                exit(1);
            }
            pos +=ret;
            len -=ret;
        }
    }
    close(readfd);

    mytbf_destroy(me);
}
/*****************
    create 1 thread, work for :

    1,open fread file, open write file
    2, read n bytes, write n bytes.

*/

void thd_createjob(int index,int cps, int burst, char* readpath, char * writepath)
{
    pthread_t tid;
    //path for argv[1] , read file path
    int writefd;
    int readfd;
    int err;





    do
    {
        readfd = open(readpath,O_RDONLY);
        if(readfd < 0 )
        {
            if(errno != EINTR)
            {
                perror("sfd open()");
                exit(1);
            }
        }
    }while(readfd < 0);

    writefd = open(writepath,O_WRONLY|O_CREAT|O_TRUNC,0700);
    if(writefd < 0)
    {
        fprintf(stderr,"add_job[%d] open error\n",__LINE__);
        exit(1);
    }


    myarg_st[index].icps = cps;
    myarg_st[index].iburst = burst;
    myarg_st[index].ireadfd = readfd;
    myarg_st[index].iwritefd = writefd;


    err = pthread_create(tid,NULL,add_job,(void *)&myarg_st[index]);
    if(err)
    {
        fprintf(stderr,"pthread_create():%s\n",strerror(err));
        exit(1);
    }

}
