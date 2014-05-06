#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
	sem_t	*sem;
	int		val;

	if (argc != 2) {
		fprintf(stderr, "usage: semwait <name>");
		exit(1);
	}

	sem = sem_open(argv[1], 0);
	sem_wait(sem);
	sem_getvalue(sem, &val);
	printf("pid %ld has semaphore, value = %d\n", (long) getpid(), val);

	pause();	/* blocks until killed */
	exit(0);
}
