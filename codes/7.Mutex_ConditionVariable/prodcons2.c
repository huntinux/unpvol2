/* include main */
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#define	MAXNITEMS 		1000000
#define	MAXNTHREADS			100

int		nitems;			/* read-only by producer and consumer */
struct {
  pthread_mutex_t	mutex;
  int	buff[MAXNITEMS];
  int	nput;
  int	nval;
} shared = { PTHREAD_MUTEX_INITIALIZER };

void	*produce(void *), *consume(void *);

#define	min(a,b)	((a) < (b) ? (a) : (b))
#define	max(a,b)	((a) > (b) ? (a) : (b))
void err_quit(const char *msg)
{
	fprintf(stderr,"%s\n",msg);
	exit(-1);
}

int
main(int argc, char **argv)
{
	int			i, nthreads, count[MAXNTHREADS];
	pthread_t	tid_produce[MAXNTHREADS], tid_consume;

	if (argc != 3)
		err_quit("usage: prodcons2 <#items> <#threads>");
	nitems = min(atoi(argv[1]), MAXNITEMS);
	nthreads = min(atoi(argv[2]), MAXNTHREADS);

	//set_concurrency(nthreads);
	pthread_setconcurrency(nthreads);
		/* 4start all the producer threads */
	for (i = 0; i < nthreads; i++) {
		count[i] = 0;
		pthread_create(&tid_produce[i], NULL, produce, &count[i]);
	}

		/* 4wait for all the producer threads */
	for (i = 0; i < nthreads; i++) {
		pthread_join(tid_produce[i], NULL);
		printf("count[%d] = %d\n", i, count[i]);	
	}

		/* 4start, then wait for the consumer thread */
	pthread_create(&tid_consume, NULL, consume, NULL);
	pthread_join(tid_consume, NULL);

	exit(0);
}
/* end main */

/* include producer */
void *
produce(void *arg)
{
	for ( ; ; ) {
		pthread_mutex_lock(&shared.mutex);
		if (shared.nput >= nitems) {
			pthread_mutex_unlock(&shared.mutex);
			return(NULL);		/* array is full, we're done */
		}
		shared.buff[shared.nput] = shared.nval;
		shared.nput++;
		shared.nval++;
		pthread_mutex_unlock(&shared.mutex);
		*((int *) arg) += 1;
	}
}

void *
consume(void *arg)
{
	int		i;

	for (i = 0; i < nitems; i++) {
		if (shared.buff[i] != i)
			printf("buff[%d] = %d\n", i, shared.buff[i]);
	}
	return(NULL);
}
/* end producer */

//  gcc prodcons2.c -o prodcons2.out -lpthread
