#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>


volatile sig_atomic_t	mqflag;		/* set nonzero by signal handler */
static void	sig_usr1(int);


void err_quit(const char *msg)
{
	fprintf(stderr,"%s\n",msg);
	exit(-1);
}


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
		err_quit("usage: mqnotifysig2 <name>");

		/* 4open queue, get attributes, allocate read buffer */
	mqd = mq_open(argv[1], O_RDONLY); // 打开message queue
	mq_getattr(mqd, &attr); // 获得queue的属性
	buff = malloc(attr.mq_msgsize); // 分配缓冲区

	sigemptyset(&zeromask);		/* no signals blocked */
	sigemptyset(&newmask);
	sigemptyset(&oldmask);
	sigaddset(&newmask, SIGUSR1); // 增加SIGUSR1 到newmask中

		/* 4establish signal handler, enable notification */
	signal(SIGUSR1, sig_usr1); // 注册SIGUSR1的处理函数
	sigev.sigev_notify = SIGEV_SIGNAL; // 当queue由空变成非空的时候，产生SIGUSR1信号
	sigev.sigev_signo = SIGUSR1;
	mq_notify(mqd, &sigev);

	for ( ; ; ) {
		sigprocmask(SIG_BLOCK, &newmask, &oldmask);	/* block SIGUSR1 *///即不处理SIGUSR1信号
		while (mqflag == 0) // 一直等待mqflag变成非0,否则，解除对SIGUSR1的阻塞，等待它到来。
			sigsuspend(&zeromask); // 临时改变mask，当SIGUSR1的handler返回是，sigsuspend也返回
		mqflag = 0;		/* reset flag */ // 重新设置为0

		mq_notify(mqd, &sigev);			/* reregister first */
		n = mq_receive(mqd, buff, attr.mq_msgsize, NULL);
		printf("read %ld bytes\n", (long) n);
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

// 存在的问题：
// 如果在第一个message被取走前，又有第二个消息进入队列。那么只会产生一个信号。
// The fundamental problem is that the notification is sent only when a message 
// is placed onto an empty queue.
// 只有当queue由空变成非空的时候才会产生notification ,参考 man mq_nofity
