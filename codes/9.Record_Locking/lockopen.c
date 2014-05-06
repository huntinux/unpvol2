
// 使用open函数实现文件锁

#define	LOCKFILE	"/tmp/seqno.lock"
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void
my_lock(int fd)
{
	int		tempfd;

	while ( (tempfd = open(LOCKFILE, O_RDWR|O_CREAT|O_EXCL, FILE_MODE)) < 0) {
		if (errno != EEXIST)
			err_sys("open error for lock file");
		/* someone else has the lock, loop around and try again */
	}
	close(tempfd);			/* opened the file, we have the lock */
}

void
my_unlock(int fd)
{
	unlink(LOCKFILE);		/* release lock by removing file */
}
