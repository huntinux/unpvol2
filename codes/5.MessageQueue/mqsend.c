#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>

typedef unsigned int  uint_t ;

void err_quit(const char *msg)
{
	fprintf(stderr,"%s\n",msg);
	exit(-1);
}

int
main(int argc, char **argv)
{
	mqd_t	mqd;
	void	*ptr;
	size_t	len;
	uint_t	prio;

	if (argc != 4)
		err_quit("usage: mqsend <name> <#bytes> <priority>");
	len = atoi(argv[2]);
	prio = atoi(argv[3]);

	mqd = mq_open(argv[1], O_WRONLY);

	ptr = calloc(len, sizeof(char));
	mq_send(mqd, ptr, len, prio);

	exit(0);
}
