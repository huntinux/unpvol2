#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdlib.h>


int
main(int argc, char **argv)
{
	if (argc != 2)
		perror("usage: mqunlink <name>");

	mq_unlink(argv[1]);

	exit(0);
}
