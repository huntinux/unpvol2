#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

mqd_t	mqd;
void	*buff;
struct mq_attr	attr;
struct sigevent	sigev;

void err_quit(const char *msg)
{
	fprintf(stderr,"%s\n",msg);
	exit(-1);
}

static void	sig_usr1(int);

int
main(int argc, char **argv)
{
	if (argc != 2)
		err_quit("usage: mqnotifysig1 <name>");

		/* 4open queue, get attributes, allocate read buffer */
	mqd = mq_open(argv[1], O_RDONLY);
	mq_getattr(mqd, &attr);
	buff = malloc(attr.mq_msgsize);

		/* 4establish signal handler, enable notification */
	signal(SIGUSR1, sig_usr1);
	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = SIGUSR1;
	mq_notify(mqd, &sigev);

	for ( ; ; )
		pause();		/* signal handler does everything */
	exit(0);
}

static void
sig_usr1(int signo)
{
	ssize_t	n;

	mq_notify(mqd, &sigev);			/* reregister first */
	n = mq_receive(mqd, buff, attr.mq_msgsize, NULL);
	printf("SIGUSR1 received, read %ld bytes\n", (long) n);
	return;
}
