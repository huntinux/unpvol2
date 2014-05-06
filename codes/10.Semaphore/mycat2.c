// 使用双缓冲区 （多缓冲区）

#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>


#define	NBUFF	 8
#define BUFFSIZE 8192

struct {	/* data shared by producer and consumer */
  struct {
    char	data[BUFFSIZE];			/* a buffer */
    ssize_t	n;						/* count of #bytes in the buffer */
  } buff[NBUFF];					/* NBUFF of these buffers/counts */
  sem_t	mutex, nempty, nstored;		/* semaphores, not pointers */
} shared;

int		fd;							/* input file to copy to stdout */
void	*produce(void *), *consume(void *);

int
main(int argc, char **argv)
{
	pthread_t	tid_produce, tid_consume;

	if (argc != 2){
		fprintf(stderr, "usage: mycat2 <pathname>");
		exit(1);
	}

	fd = open(argv[1], O_RDONLY);

		/* 4initialize three semaphores */
	sem_init(&shared.mutex, 0, 1);
	sem_init(&shared.nempty, 0, NBUFF);
	sem_init(&shared.nstored, 0, 0);

		/* 4one producer thread, one consumer thread */
	pthread_setconcurrency(2);
	pthread_create(&tid_produce, NULL, produce, NULL);	/* reader thread */
	pthread_create(&tid_consume, NULL, consume, NULL);	/* writer thread */

	pthread_join(tid_produce, NULL);
	pthread_join(tid_consume, NULL);

	sem_destroy(&shared.mutex);
	sem_destroy(&shared.nempty);
	sem_destroy(&shared.nstored);
	exit(0);
}
/* end main */

/* include prodcons */
void *
produce(void *arg)
{
	int		i;

	for (i = 0; ; ) {
		sem_wait(&shared.nempty);	/* wait for at least 1 empty slot */

		sem_wait(&shared.mutex);
			/* 4critical region */
		sem_post(&shared.mutex);

		shared.buff[i].n = read(fd, shared.buff[i].data, BUFFSIZE);
		if (shared.buff[i].n == 0) {
			sem_post(&shared.nstored);	/* 1 more stored item */
			return(NULL);
		}
		if (++i >= NBUFF)
			i = 0;					/* circular buffer */

		sem_post(&shared.nstored);	/* 1 more stored item */
	}
}

void *
consume(void *arg)
{
	int		i;

	for (i = 0; ; ) {
		sem_wait(&shared.nstored);		/* wait for at least 1 stored item */

		sem_wait(&shared.mutex);
			/* 4critical region */
		sem_post(&shared.mutex);

		if (shared.buff[i].n == 0)
			return(NULL);
		write(STDOUT_FILENO, shared.buff[i].data, shared.buff[i].n);
		if (++i >= NBUFF)
			i = 0;					/* circular buffer */

		sem_post(&shared.nempty);		/* 1 more empty slot */
	}
}
/* end prodcons */
