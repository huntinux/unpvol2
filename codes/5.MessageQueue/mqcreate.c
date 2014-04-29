#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdlib.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
	
struct mq_attr	attr;	/* mq_maxmsg and mq_msgsize both init to 0 */

int
main(int argc, char **argv)
{
	int		c, flags;
	mqd_t	mqd;

	flags = O_RDWR | O_CREAT;
	while ( (c = getopt(argc, argv, "em:z:")) != -1) {
		switch (c) {
		case 'e':
			flags |= O_EXCL;
			break;

		case 'm':
			attr.mq_maxmsg = atol(optarg);
			break;

		case 'z':
			attr.mq_msgsize = atol(optarg);
			break;
		}
	}
	if (optind != argc - 1)
		perror("usage: mqcreate [ -e ] [ -m maxmsg -z msgsize ] <name>");

	if ((attr.mq_maxmsg != 0 && attr.mq_msgsize == 0) ||
		(attr.mq_maxmsg == 0 && attr.mq_msgsize != 0))
		perror("must specify both -m maxmsg and -z msgsize");

	mqd = mq_open(argv[optind], flags, FILE_MODE,
				  (attr.mq_maxmsg != 0) ? &attr : NULL);

	mq_close(mqd);
	exit(0);
}
