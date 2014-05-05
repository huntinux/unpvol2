#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define	MAXNTHREADS	100

void	*reader(void *), *writer(void *);

int		nloop = 1000, nreaders = 6, nwriters = 4;

struct {
  pthread_rwlock_t	rwlock;
  pthread_mutex_t	rcountlock;
  int				nreaders;
  int				nwriters;
} shared = { PTHREAD_RWLOCK_INITIALIZER, PTHREAD_MUTEX_INITIALIZER };


int
main(int argc, char **argv)
{
	int		c, i;
	pthread_t	tid_readers[MAXNTHREADS], tid_writers[MAXNTHREADS];

	while ( (c = getopt(argc, argv, "n:r:w:")) != -1) {
		switch (c) {
		case 'n':
			nloop = atoi(optarg);
			break;

		case 'r':
			nreaders = atoi(optarg);
			break;

		case 'w':
			nwriters = atoi(optarg);
			break;
		}
	}
	if (optind != argc) {
		fprintf(stderr, "usage: test1 [-n #loops ] [ -r #readers ] [ -w #writers ]");
		exit(-1);
	}

		/* 4create all the reader and writer threads */
	pthread_setconcurrency(nreaders + nwriters);
	for (i = 0; i < nreaders; i++)
		pthread_create(&tid_readers[i], NULL, reader, NULL);
	for (i = 0; i < nwriters; i++)
		pthread_create(&tid_writers[i], NULL, writer, NULL);

		/* wait for all the threads to complete */
	for (i = 0; i < nreaders; i++)
		pthread_join(tid_readers[i], NULL);
	for (i = 0; i < nwriters; i++)
		pthread_join(tid_writers[i], NULL);

	exit(0);
}

void *
reader(void *arg)
{
	int		i;

	for (i = 0; i < nloop; i++) {
		pthread_rwlock_rdlock(&shared.rwlock);

		pthread_mutex_lock(&shared.rcountlock);
		shared.nreaders++;	/* shared by all readers; must protect */
		pthread_mutex_unlock(&shared.rcountlock);

		if (shared.nwriters > 0){
			fprintf(stderr,"reader: %d writers found", shared.nwriters);
			exit(-1);
		}

		pthread_mutex_lock(&shared.rcountlock);
		shared.nreaders--;	/* shared by all readers; must protect */
		pthread_mutex_unlock(&shared.rcountlock);

		pthread_rwlock_unlock(&shared.rwlock);
	}
	return(NULL);
}

void *
writer(void *arg)
{
	int		i;

	for (i = 0; i < nloop; i++) {
		pthread_rwlock_wrlock(&shared.rwlock);
		shared.nwriters++;	/* only one writer; need not protect */

		if (shared.nwriters > 1) {
			fprintf(stderr, "writer: %d writers found", shared.nwriters);
			exit(-1);
		}
		if (shared.nreaders > 0){
			fprintf(stderr, "writer: %d readers found", shared.nreaders);
			exit(-1);
		}

		shared.nwriters--;	/* only one writer; need not protect */
		pthread_rwlock_unlock(&shared.rwlock);
	}
	return(NULL);
}
