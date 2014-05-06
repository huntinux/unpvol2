#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int
main(int argc, char **argv)
{
	int		c, flags;
	sem_t	*sem;
	unsigned int	value;

	flags = O_RDWR | O_CREAT;
	value = 1;
	while ( (c = getopt(argc, argv, "ei:")) != -1) {
		switch (c) {
		case 'e':
			flags |= O_EXCL;
			break;

		case 'i':
			value = atoi(optarg);
			break;
		}
	}
	if (optind != argc - 1) {
		fprintf(stderr, "usage: semcreate [ -e ] [ -i initalvalue ] <name>");
		exit(1);
	}

	sem = sem_open(argv[optind], flags, FILE_MODE, value);

	sem_close(sem);
	exit(0);
}

// gcc *.c -lpthread
