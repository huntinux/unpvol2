#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdlib.h>


int
main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "usage: shmunlink <name>");
		exit(1);
	}

	shm_unlink(argv[1]);

	exit(0);
}
