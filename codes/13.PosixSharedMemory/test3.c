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
	int		fd1, fd2, *ptr1, *ptr2;
	pid_t	childpid;
	struct stat	stat;

	if (argc != 2){
		fprintf(stderr, "usage: test3 <name>");
		exit(1);
	}

	shm_unlink(argv[1]);
	fd1 = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, FILE_MODE);
	ftruncate(fd1, sizeof(int));
	fd2 = open("/etc/motd", O_RDONLY);
	fstat(fd2, &stat);

	if ( (childpid = fork()) == 0) {
			/* 4child */
		ptr2 = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd2, 0);
		ptr1 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
					MAP_SHARED, fd1, 0);
		printf("child: shm ptr = %p, motd ptr = %p\n", ptr1, ptr2);

		sleep(5);
		printf("shared memory integer = %d\n", *ptr1);
		exit(0);
	}
		/* 4parent: mmap in reverse order from child */
	ptr1 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
	ptr2 = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd2, 0);
	printf("parent: shm ptr = %p, motd ptr = %p\n", ptr1, ptr2);
	*ptr1 = 777;
	waitpid(childpid, NULL, 0);

	exit(0);
}
