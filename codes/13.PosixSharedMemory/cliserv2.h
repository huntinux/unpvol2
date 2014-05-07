#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


#define	MESGSIZE	256		/* max #bytes per message, incl. null at end */
#define	NMESG		 16		/* max #messages */

struct shmstruct {		/* struct stored in shared memory */
  sem_t	mutex;			/* three Posix memory-based semaphores */
  sem_t	nempty;
  sem_t	nstored;
  int	nput;			/* index into msgoff[] for next put */
  long	noverflow;		/* #overflows by senders */
  sem_t	noverflowmutex;	/* mutex for noverflow counter */
  long	msgoff[NMESG];	/* offset in shared memory of each message */
  char	msgdata[NMESG * MESGSIZE];	/* the actual messages */
};

/*
Overflow counter
The possibility exists that a client wants to send a message but all the message slots
are taken. But if the client is actually a server of some type (perhaps an FTP server or an
HTTP server), the client does not want to wait for the server to free up a slot. Therefore,
we will write our clients so that they do not block but increment the noverflow
counter when this happens. Since this overflow counter is also shared among all the
clients and the server, it too requires a mutex so that its value is not corrupted.
*/

int
sleep_us(unsigned int nusecs)
{
	struct timeval	tval;

	if (nusecs == 0)
		return(0);

	for ( ; ; ) {
		tval.tv_sec = nusecs / 1000000;
		tval.tv_usec = nusecs % 1000000;
		if (select(0, NULL, NULL, NULL, &tval) == 0)
			return(0);		/* all OK */
		/*
		 * Note than on an interrupted system call there's not
		 * much we can do, since the timeval{} isn't updated with the time
		 * remaining.  We could obtain the clock time before the call, and
		 * then obtain the clock time here, subtracting them to determine
		 * how long select() blocked before it was interrupted, but that
		 * seems like too much work :-)
		 */
		if (errno != EINTR)
			return(-1);
		/* else go around again */
	}
}


