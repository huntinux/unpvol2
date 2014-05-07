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
	unsigned char	*ptr;

	if (argc != 2) {
		fprintf(stderr, "usage: shmwrite <name>");
		exit(1);
	}

		/* 4open, get size, map */
	fd = shm_open(argv[1], O_RDWR, FILE_MODE);
	fstat(fd, &stat);
	ptr = mmap(NULL, stat.st_size, PROT_READ | PROT_WRITE,
			   MAP_SHARED, fd, 0);
	close(fd);

		/* 4set: ptr[0] = 0, ptr[1] = 1, etc. */
	for (i = 0; i < stat.st_size; i++)
		*ptr++ = i % 256;

	exit(0);
}
