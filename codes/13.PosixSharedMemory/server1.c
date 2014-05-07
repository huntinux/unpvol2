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
	int		fd;
	struct shmstruct	*ptr;

	if (argc != 3) {
		fprintf(stderr, "usage: server1 <shmname> <semname>");
		exit(1);
	}

	shm_unlink(argv[1]);		/* OK if this fails */
		/* 4create shm, set its size, map it, close descriptor */
	fd = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, FILE_MODE);
	ftruncate(fd, sizeof(struct shmstruct));
	ptr = mmap(NULL, sizeof(struct shmstruct), PROT_READ | PROT_WRITE,
			   MAP_SHARED, fd, 0);
	close(fd);

	sem_unlink(argv[2]);		/* OK if this fails */
	mutex = sem_open(argv[2], O_CREAT | O_EXCL, FILE_MODE, 1);
	sem_close(mutex);

	exit(0);
}

// gcc server1.c -o server1 lrt
// ./server1 /tmpshm /tmpsem 
// /tmpshm /tmpsem 的位置在 /dev/shm中。将man shm_open
