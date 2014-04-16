/* client 发送PID，pathname给server
	然后，从fifo.pid中读取文件内容 
 */
#include  <stdio.h>
#include  <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

const char * serverfifo = "/tmp/server.fifo"


int main(int argc, char *argv[])
{
	int sfd, cfd;
	pid_t pid;
	char buf[64];
	int rnum;

	// 参数检查
	if( argc != 2 )
	{
		printf("Usage: client pathname");
		exit(-1);
	}


	// 只写方式打开server.fifo，把pathname传送过去。
	if((sfd = open(serverfifo, O_WRONLY)) == -1)
	{
		perror("open server fifo error");
		exit(-1);
	}
	if(write(sdf, argv[1], sizeof(argv[1])) == -1)
	{
		perror("write server fifo error");
		exit(-1);
	}

	// 关闭server.fifo
	close(sfd);

	// 创建接收fifo.pid,读取文件内容。
	pid = getpid();
	snprintf(buf, sizeof(buf),"/tmp/client.%d.fifo", pid);
	if(mkfifo(buf, O_CREAT|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) == -1)
	{
		perror("mkfifo error");
		exit(-1);
	}

	if((cfd = open(serverfifo, O_WRONLY)) == -1)
	{
		perror("open client fifo error");
		exit(-1);
	}

	while((rnum = read(cfd, buf, sizeof(buf))) > 0)
	{
		write(1, buf, rnum);
	}

	// 关闭fifo.pid
	close(cfd);

	
	return 0;
}
