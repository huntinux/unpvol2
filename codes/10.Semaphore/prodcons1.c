// posix named semaphore
// have a path name with the semaphore

#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#define	NBUFF	 10
#define	SEM_MUTEX	"mutex"	 	/* these are args to px_ipc_name() */
#define	SEM_NEMPTY	"nempty"
#define	SEM_NSTORED	"nstored"

int		nitems;					/* read-only by producer and consumer */
struct {	/* data shared by producer and consumer */
  int	buff[NBUFF];
  sem_t	*mutex, *nempty, *nstored;
} shared;

void	*produce(void *), *consume(void *);

const char sem_mutex[] = "/semmutex";
const char sem_nempty[] = "/semnempty";
const char sem_nstored[] = "/semnstored";

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int
main(int argc, char **argv)
{
	pthread_t	tid_produce, tid_consume;

	if (argc != 2){
		fprintf(stderr, "usage: prodcons1 <#items>");
		exit(1);
	}
	nitems = atoi(argv[1]);

		/* 4create three semaphores */
	shared.mutex = sem_open(sem_mutex, O_CREAT | O_EXCL,
							FILE_MODE, 1);
	shared.nempty = sem_open(sem_nempty, O_CREAT | O_EXCL,
							 FILE_MODE, NBUFF);
	shared.nstored = sem_open(sem_nstored, O_CREAT | O_EXCL,
							  FILE_MODE, 0);

		/* 4create one producer thread and one consumer thread */
	pthread_setconcurrency(2);
	pthread_create(&tid_produce, NULL, produce, NULL);
	pthread_create(&tid_consume, NULL, consume, NULL);

		/* 4wait for the two threads */
	pthread_join(tid_produce, NULL);
	pthread_join(tid_consume, NULL);

		/* 4remove the semaphores */
	sem_unlink(sem_mutex);
	sem_unlink(sem_nempty);
	sem_unlink(sem_nstored);
	exit(0);
}
/* end main */

/* include prodcons */
void *
produce(void *arg)
{
	int		i;

	for (i = 0; i < nitems; i++) {
		sem_wait(shared.nempty);	/* wait for at least 1 empty slot */
		sem_wait(shared.mutex);
		shared.buff[i % NBUFF] = i;	/* store i into circular buffer */
		printf("producer put %d\n", shared.buff[i % NBUFF]);
		sem_post(shared.mutex);
		sem_post(shared.nstored);	/* 1 more stored item */
	}
	return(NULL);
}

void *
consume(void *arg)
{
	int		i;

	for (i = 0; i < nitems; i++) {
		sem_wait(shared.nstored);		/* wait for at least 1 stored item */
		sem_wait(shared.mutex);
		if (shared.buff[i % NBUFF] != i)
			printf("buff[%d] = %d\n", i, shared.buff[i % NBUFF]);
		else
			printf("consumer get %d\n", shared.buff[i % NBUFF]);
		sem_post(shared.mutex);
		sem_post(shared.nempty);		/* 1 more empty slot */
	}
	return(NULL);
}
/* end prodcons */
