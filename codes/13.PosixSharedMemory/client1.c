#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdlib.h>
#include <semaphore.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


struct shmstruct {	/* struct stored in shared memory */
  int	count;
};
sem_t	*mutex;		/* pointer to named semaphore */

int
main(int argc, char **argv)
{
	int		fd, i, nloop;
	pid_t	pid;
	struct shmstruct	*ptr;

	if (argc != 4) {
		fprintf(stderr, "usage: client1 <shmname> <semname> <#loops>");
		exit(1);
	}

	nloop = atoi(argv[3]);

	fd = shm_open((argv[1]), O_RDWR, FILE_MODE);
	ptr = mmap(NULL, sizeof(struct shmstruct), PROT_READ | PROT_WRITE,
			   MAP_SHARED, fd, 0);
	close(fd);

	mutex = sem_open((argv[2]), 0);

	pid = getpid();
	for (i = 0; i < nloop; i++) {
		sem_wait(mutex);
		printf("pid %ld: %d\n", (long) pid, ptr->count++);
		sem_post(mutex);
	}
	exit(0);
}
