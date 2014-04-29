#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>

// mq_receive returns the oldest message of the highest priority
// priority: the smaller , the higher : 数字越小，优先级越高

typedef unsigned int  uint_t;

void err_quit(const char *msg)
{
	fprintf(stderr,"%s\n",msg);
	exit(-1);
}

int
main(int argc, char **argv)
{
	int		c, flags;
	mqd_t	mqd;
	ssize_t	n;
	uint_t	prio;
	void	*buff;
	struct mq_attr	attr;

	flags = O_RDONLY;
	while ( (c = getopt(argc, argv, "n")) != -1) {
		switch (c) {
		case 'n':
			flags |= O_NONBLOCK;
			break;
		}
	}
	if (optind != argc - 1)
		err_quit("usage: mqreceive [ -n ] <name>");

	mqd = mq_open(argv[optind], flags);
	mq_getattr(mqd, &attr);

	buff = malloc(attr.mq_msgsize);

	n = mq_receive(mqd, buff, attr.mq_msgsize, &prio);
	printf("read %ld bytes, priority = %u\n", (long) n, prio);

	exit(0);
}
