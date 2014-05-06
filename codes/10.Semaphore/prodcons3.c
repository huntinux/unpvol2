// multi producer ,one consumer
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define	NBUFF	 	 10
#define	MAXNTHREADS	100

int		nitems, nproducers;		/* read-only by producer and consumer */

struct {	/* data shared by producers and consumer */
  int	buff[NBUFF];
  int	nput;
  int	nputval;
  sem_t	mutex, nempty, nstored;		/* semaphores, not pointers */
} shared;

void	*produce(void *), *consume(void *);

#define min(x,y) ((x) > (y) ? (y) : (x))

int
main(int argc, char **argv)
{
	int		i, count[MAXNTHREADS];
	pthread_t	tid_produce[MAXNTHREADS], tid_consume;

	if (argc != 3) {
		fprintf(stderr, "usage: prodcons3 <#items> <#producers>");
		exit(1);
	}
	nitems = atoi(argv[1]);
	nproducers = min(atoi(argv[2]), MAXNTHREADS);

		/* 4initialize three semaphores */
	sem_init(&shared.mutex, 0, 1);
	sem_init(&shared.nempty, 0, NBUFF);
	sem_init(&shared.nstored, 0, 0);

		/* 4create all producers and one consumer */
	pthread_setconcurrency(nproducers + 1);
	for (i = 0; i < nproducers; i++) {
		count[i] = 0;
		pthread_create(&tid_produce[i], NULL, produce, &count[i]);
	}
	pthread_create(&tid_consume, NULL, consume, NULL);

		/* 4wait for all producers and the consumer */
	for (i = 0; i < nproducers; i++) {
		pthread_join(tid_produce[i], NULL);
		printf("count[%d] = %d\n", i, count[i]);	
	}
	pthread_join(tid_consume, NULL);

	sem_destroy(&shared.mutex);
	sem_destroy(&shared.nempty);
	sem_destroy(&shared.nstored);
	exit(0);
}
/* end main */

/* include produce */
void *
produce(void *arg)
{
	for ( ; ; ) {
		sem_wait(&shared.nempty);	/* wait for at least 1 empty slot */
		sem_wait(&shared.mutex);

		if (shared.nput >= nitems) {
			sem_post(&shared.nempty);
			sem_post(&shared.mutex);
			return(NULL);			/* all done */
		}
		
		printf("producer[%ld] put %d\n", syscall(SYS_gettid), shared.nputval);
		shared.buff[shared.nput % NBUFF] = shared.nputval;
		shared.nput++;
		shared.nputval++;

		sem_post(&shared.mutex);
		sem_post(&shared.nstored);	/* 1 more stored item */
		*((int *) arg) += 1;
	}
}
/* end produce */

/* include consume */
void *
consume(void *arg)
{
	int		i;

	for (i = 0; i < nitems; i++) {
		sem_wait(&shared.nstored);		/* wait for at least 1 stored item */
		sem_wait(&shared.mutex);

		if (shared.buff[i % NBUFF] != i)
			printf("error: buff[%d] = %d\n", i, shared.buff[i % NBUFF]);
		else
			printf("consumer get %d\n", shared.buff[i % NBUFF]);

		sem_post(&shared.mutex);
		sem_post(&shared.nempty);		/* 1 more empty slot */
	}
	return(NULL);
}
/* end consume */
