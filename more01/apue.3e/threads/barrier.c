/*****
    cc barrier.c ../lib/error.c  -o barrier -pthread -lrt -lbsd

    heapsort need install libbsd-dev.
*/


#include "apue.h"
#include <pthread.h>
#include <limits.h>
#include <sys/time.h>

#define NTHR   8				/* number of threads */
#define NUMNUM 8000000L			/* number of numbers to sort */
#define TNUM   (NUMNUM/NTHR)	/* number to sort per thread */

long nums[NUMNUM];      //not sort  array , 8000000 elements
long snums[NUMNUM];     //  part  sort  array , 8000000 elements

pthread_barrier_t b;

#ifdef SOLARIS
#define heapsort qsort
#else
extern int heapsort(void *, size_t, size_t,
                    int (*)(const void *, const void *));
#endif

/*
 * Compare two long integers (helper function for heapsort)
 */
int
complong(const void *arg1, const void *arg2)
{
	long l1 = *(long *)arg1;
	long l2 = *(long *)arg2;

	if (l1 == l2)
		return 0;
	else if (l1 < l2)
		return -1;
	else
		return 1;
}

/*
 * Worker thread to sort a portion of the set of numbers.
 */
void *
thr_fn(void *arg)
{
	long	idx = (long)arg;

	heapsort(&nums[idx], TNUM, sizeof(long), complong);     //worker0 = 0- 999999, worker1 = 1000000 - 1999999,...
	pthread_barrier_wait(&b);       // wroker thread waiting point

	/*
	 * Go off and perform more work ...
	 */
	return((void *)0);
}

/*
 * Merge the results of the individual sorted ranges.
 */
void
merge()
{
	long	idx[NTHR];      //array have 8 long elements, start index.
	long	i, minidx, sidx, num;

	for (i = 0; i < NTHR; i++)
		idx[i] = i * TNUM;          // start index ,, idx[0] = 0, idx[1] = 1000000, idx[2] = 2000000, ...
	for (sidx = 0; sidx < NUMNUM; sidx++) {         //0- 8000000   search all array,    8 sub segments are  ordered.
		num = LONG_MAX;
		for (i = 0; i < NTHR; i++) {
			if ((idx[i] < (i+1)*TNUM) && (nums[idx[i]] < num)) {
				num = nums[idx[i]];
				minidx = i;
			}
		}
		snums[sidx] = nums[idx[minidx]];
		idx[minidx]++;
	}
}

int
main()
{
	unsigned long	i;
	struct timeval	start, end;
	long long		startusec, endusec;
	double			elapsed;
	int				err;
	pthread_t		tid;

	/*
	 * Create the initial set of numbers to sort.
	 */
	srandom(1);
	for (i = 0; i < NUMNUM; i++)
		nums[i] = random();

	/*
	 * Create 8 threads to sort the numbers.
	 */
	gettimeofday(&start, NULL);
	pthread_barrier_init(&b, NULL, NTHR+1);     //init, barrier's count = 8+1 , 1 main thread + 8 workers
	for (i = 0; i < NTHR; i++) {
		err = pthread_create(&tid, NULL, thr_fn, (void *)(i * TNUM));   //0,1000000,2000000....
		if (err != 0)
			err_exit(err, "can't create thread");
	}
	pthread_barrier_wait(&b);       // main thread waiting point
	merge();
	gettimeofday(&end, NULL);

	/*
	 * Print the sorted list.
	 */
	startusec = start.tv_sec * 1000000 + start.tv_usec;
	endusec = end.tv_sec * 1000000 + end.tv_usec;
	elapsed = (double)(endusec - startusec) / 1000000.0;


	for (i = 0; i < NUMNUM; i++)
		printf("%ld\n", snums[i]);

    printf("sort took %.4f seconds\n", elapsed);
	exit(0);
}
