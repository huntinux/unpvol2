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
		fprintf(stderr, "usage: semgetvalue <name>");
		exit(1);
	}

	sem = sem_open(argv[1], 0);
	sem_getvalue(sem, &val);
	printf("value = %d\n", val);

	exit(0);
}
