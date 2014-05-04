#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

void err_quit(const char *msg)
{
	fprintf(stderr,"%s\n",msg);
	exit(-1);
}

volatile sig_atomic_t	mqflag;		/* set nonzero by signal handler */
static void	sig_usr1(int);

int
main(int argc, char **argv)
{
	mqd_t	mqd;
	void	*buff;
	ssize_t	n;
	sigset_t	zeromask, newmask, oldmask;
	struct mq_attr	attr;
	struct sigevent	sigev;

	if (argc != 2)
		err_quit("usage: mqnotifysig3 <name>");

		/* 4open queue, get attributes, allocate read buffer */
	mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
	mq_getattr(mqd, &attr);
	buff = malloc(attr.mq_msgsize);

	sigemptyset(&zeromask);		/* no signals blocked */
	sigemptyset(&newmask);
	sigemptyset(&oldmask);
	sigaddset(&newmask, SIGUSR1);
		/* 4establish signal handler, enable notification */
	signal(SIGUSR1, sig_usr1);
	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = SIGUSR1;
	mq_notify(mqd, &sigev);

	for ( ; ; ) {
		sigprocmask(SIG_BLOCK, &newmask, &oldmask);	/* block SIGUSR1 */
		while (mqflag == 0)
			sigsuspend(&zeromask);
		mqflag = 0;		/* reset flag */

		mq_notify(mqd, &sigev);			/* reregister first */
		while ( (n = mq_receive(mqd, buff, attr.mq_msgsize, NULL)) >= 0) {
			printf("read %ld bytes\n", (long) n);
		}
		if (errno != EAGAIN)
			err_sys("mq_receive error");
		sigprocmask(SIG_UNBLOCK, &newmask, NULL);	/* unblock SIGUSR1 */
	}
	exit(0);
}

static void
sig_usr1(int signo)
{
	mqflag = 1;
	return;
}

// 每次取出所有message
