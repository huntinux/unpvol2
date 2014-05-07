#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdlib.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int
main(int argc, char **argv)
{
	int		c, fd, flags;
	char	*ptr;
	off_t	length;

	flags = O_RDWR | O_CREAT;
	while ( (c = getopt(argc, argv, "e")) != -1) {
		switch (c) {
		case 'e':
			flags |= O_EXCL;
			break;
		}
	}
	if (optind != argc - 2) {
		fprintf(stderr, "usage: shmcreate [ -e ] <name> <length>");
		exit(1);
	}
	length = atoi(argv[optind + 1]);

	fd = shm_open(argv[optind], flags, FILE_MODE);
	ftruncate(fd, length);

	ptr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	exit(0);
}
