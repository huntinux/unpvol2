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
	int		i, fd;
	struct stat	stat;
	unsigned char	c, *ptr;

	if (argc != 2) {
		fprintf(stderr, "usage: shmread <name>");
		exit(1);
	}

		/* 4open, get size, map */
	fd = shm_open(argv[1], O_RDONLY, FILE_MODE);
	fstat(fd, &stat);
	ptr = mmap(NULL, stat.st_size, PROT_READ,
			   MAP_SHARED, fd, 0);
	close(fd);

		/* 4check that ptr[0] = 0, ptr[1] = 1, etc. */
	for (i = 0; i < stat.st_size; i++)
		if ( (c = *ptr++) != (i % 256))
			fprintf(stderr, "ptr[%d] = %d", i, c);

	exit(0);
}
