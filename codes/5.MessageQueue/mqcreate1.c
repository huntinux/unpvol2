#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdlib.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
	
int
main(int argc, char **argv)
{
	int		c, flags;
	mqd_t	mqd;

	flags = O_RDWR | O_CREAT;
	while ( (c = getopt(argc, argv, "e")) != -1) {
		switch (c) {
		case 'e':
			flags |= O_EXCL;
			break;
		}
	}
	if (optind != argc - 1)
		perror("usage: mqcreate [ -e ] <name>");

	mqd = mq_open(argv[optind], flags, FILE_MODE, NULL);

	mq_close(mqd);
	exit(0);
}

// gcc mqcreate1.c -lrt
//Mounting the message queue file system
//       On Linux, message queues are created in a virtual file system.  (Other implementations may
//       also provide such a feature, but the details are likely to differ.)  This file system  can
//       be mounted (by the superuser) using the following commands:
//
//           # mkdir /dev/mqueue
//           # mount -t mqueue none /dev/mqueue
//
//       The sticky bit is automatically enabled on the mount directory.
//
//       After the file system has been mounted, the message queues on the system can be viewed and
//       manipulated using the commands usually used for files (e.g., ls(1) and rm(1)).
