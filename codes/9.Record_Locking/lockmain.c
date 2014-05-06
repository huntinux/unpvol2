#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*

	记录锁：使用fcntl函数
	程序会读取文件seqno, 读出里面的数字，然后加1，在写到文件中

 */


#define	SEQFILE	"seqno"		/* filename */
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define	MAXLINE		4096	/* max text line length */

void
my_lock(int fd)
{
	struct flock	lock;

	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;				/* write lock entire file */

	fcntl(fd, F_SETLKW, &lock);
}

void
my_unlock(int fd)
{
	struct flock	lock;

	lock.l_type = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;				/* unlock entire file */

	fcntl(fd, F_SETLK, &lock);
}

int
main(int argc, char **argv)
{
	int		fd;
	long	i, seqno;
	pid_t	pid;
	ssize_t	n;
	char	line[MAXLINE + 1];

	pid = getpid();
	fd = open(SEQFILE, O_RDWR, FILE_MODE);

	for (i = 0; i < 20; i++) {
		my_lock(fd);				/* lock the file */

		lseek(fd, 0L, SEEK_SET);	/* rewind before read */
		n = read(fd, line, MAXLINE);
		line[n] = '\0';				/* null terminate for sscanf */

		n = sscanf(line, "%ld\n", &seqno);
		printf("%s: pid = %ld, seq# = %ld\n", argv[0], (long) pid, seqno);

		seqno++;					/* increment sequence number */

		snprintf(line, sizeof(line), "%ld\n", seqno);
		lseek(fd, 0L, SEEK_SET);	/* rewind before write */
		write(fd, line, strlen(line));

		my_unlock(fd);				/* unlock the file */
	}
	exit(0);
}
