
// 使用shared memory
// 将一个文件映射到shared memory
// 对share memory的修改，会有kernel负责更新到文件上。

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/mman.h> // for mmap

#define	SEM_NAME	"/mysem"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int
main(int argc, char **argv)
{
	int		fd, i, nloop, zero = 0;
	int		*ptr;
	sem_t	*mutex;

	if (argc != 3) {
		fprintf(stderr, "usage: incr2 <pathname> <#loops>");
		exit(1);
	}
	nloop = atoi(argv[2]);

		/* 4open file, initialize to 0, map into memory */
	fd = open(argv[1], O_RDWR | O_CREAT, FILE_MODE);
	write(fd, &zero, sizeof(int));
	ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);

		/* 4create, initialize, and unlink semaphore */
	mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, FILE_MODE, 1);
	sem_unlink(SEM_NAME);

	setbuf(stdout, NULL);	/* stdout is unbuffered */
	if (fork() == 0) {		/* child */
		for (i = 0; i < nloop; i++) {
			sem_wait(mutex);
			printf("child: %d\n", (*ptr)++);
			sem_post(mutex);
		}
		exit(0);
	}

		/* 4parent */
	for (i = 0; i < nloop; i++) {
		sem_wait(mutex);
		printf("parent: %d\n", (*ptr)++);
		sem_post(mutex);
	}
	exit(0);
}
