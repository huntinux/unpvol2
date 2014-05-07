
// fork之后child process 得到一份 父进程的拷贝（当然有全局变量count的一份拷贝）

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>

#define	SEM_NAME	"/mysem"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int		count = 0;

int
main(int argc, char **argv)
{
	int		i, nloop;
	sem_t	*mutex;

	if (argc != 2){
		fprintf(stderr, "usage: incr1 <#loops>");
		exit(1);
	}
	nloop = atoi(argv[1]);

		/* 4create, initialize, and unlink semaphore */
	mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, FILE_MODE, 1);
	sem_unlink(SEM_NAME);

	setbuf(stdout, NULL);	/* stdout is unbuffered */
	if (fork() == 0) {		/* child */
		for (i = 0; i < nloop; i++) {
			sem_wait(mutex);
			printf("child: %d\n", count++);
			sem_post(mutex);
		}
		exit(0);
	}

		/* 4parent */
	for (i = 0; i < nloop; i++) {
		sem_wait(mutex);
		printf("parent: %d\n", count++);
		sem_post(mutex);
	}
	exit(0);
}
