
/*********************

    multi thread
    build:cc hello_multi.c -o hello_multi  -lpthread
*/

#include<stdio.h>
#include<pthread.h>
#include<ctype.h>
#define	NUM	5

struct arg_set {
		/* two values in one arg*/
		char *fname;	/* file to examine	*/
		int  count;	/* number of words	*/

};

struct arg_set  *mailbox = NULL;        //critical resource

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t  flag = PTHREAD_COND_INITIALIZER;

main(int ac, char *av[])
{

	pthread_t      t1, t2;		/* two threads */
	struct arg_set args1, args2;	/* two argsets */
	void	       *count_words(void *);
	int            reports_in = 0;
	int	            total_words = 0;

	if ( ac != 3 ){

		printf("usage: %s file1 file2\n", av[0]);
		exit(1);

    }
	pthread_mutex_lock(&lock);	/* lock the report box now */

	args1.fname = av[1];
	args1.count = 0;
	pthread_create(&t1, NULL, count_words, (void *) &args1);

	args2.fname = av[2];
	args2.count = 0;
	pthread_create(&t2, NULL, count_words, (void *) &args2);

	while( reports_in < 2 ){

        // loop wait for 2 reports
		printf("MAIN: waiting for flag to go up\n");

		// condition flag is a shared value, should have lock.
		//1,release lock(should lock before), 2, wait flag change. 3,lock again
		pthread_cond_wait(&flag, &lock);    /* block main thread , and wait t1 or t2's notify, */

		printf("MAIN: Wow! flag was raised, I have the lock\n");
		printf("%7d: %s\n", mailbox->count, mailbox->fname);

		total_words += mailbox->count;

		if ( mailbox == &args1)
			pthread_join(t1,NULL);              //wait t1 terminate and return

		if ( mailbox == &args2)
			pthread_join(t2,NULL);

		mailbox = NULL;                         //reset ,wait another thread return value
		pthread_cond_signal(&flag);	            /* announce flag change , and main thread wake up  */
		reports_in++;

}
	printf("%7d: total words\n", total_words);

}
void *count_words(void *a)
{

	struct arg_set *args = a;	/* cast arg back to correct type */
	FILE *fp;
	int  c, prevc = '\0';

	if ( (fp = fopen(args->fname, "r")) != NULL ){

		while( ( c = getc(fp)) != EOF ){

			if ( !isalnum(c) && isalnum(prevc) )
				args->count++;
			prevc = c;

        }
		fclose(fp);

    } else
		perror(args->fname);


    printf("COUNT: waiting to get lock\n");
    pthread_mutex_lock(&lock);	    /* get the mailbox  access, critical resource*/

    printf("COUNT: have lock, storing data\n");
    if ( mailbox != NULL ){

        printf("COUNT: oops..mailbox not empty. wait for signal\n");
        pthread_cond_wait(&flag,&lock);

    }

	mailbox = args;			        /* put ptr to our args there */

	printf("COUNT: raising flag\n");
	pthread_cond_signal(&flag);	    /* raise the flag, will wake up another main wait thread */

	printf("COUNT: unlocking box\n");
	pthread_mutex_unlock(&lock);	/* release the mailbox , release critical resource */

	return NULL;

}
