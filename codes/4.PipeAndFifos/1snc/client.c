/* 
	client 发送PID，pathname给server
	然后，从fifo.pid中读取文件内容 
 */
#include  <stdio.h>
#include  <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const char * serverfifo = "/tmp/server.fifo";


int main(int argc, char *argv[])
{
	int sfd, cfd;
	pid_t pid;
	char buf[64], clientfifo[64];
	int rnum;

	if( argc != 2 )
	{
		printf("Usage: client pathname\n");
		exit(-1);
	}

	// create client fifo using pid
	pid = getpid();
	snprintf(clientfifo, sizeof(clientfifo), "/tmp/client.%d.fifo", pid);
	printf("client:client fifo is %s\n", clientfifo);
	if(mkfifo(clientfifo, O_CREAT|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) == -1)
	{
		perror("cilent:make client fifo error");
	}else{
		printf("create client fifo successfully.\n");
	}

	printf("client:client pid:%d\npathname:%s\n", pid, argv[1]);

	// open server.fifo in writeonly mode 
	// send pid and pathname to it
	if((sfd = open(serverfifo, O_WRONLY)) == -1)
	{
		perror("client:open server fifo error");
		exit(-1);
	}else{
		printf("open server fifo successfully\n");
	}

	snprintf(buf, sizeof(buf), "%d %s", pid, argv[1]);
	printf("client:buf:%s\n", buf);
	if(write(sfd, buf, sizeof(buf)) == -1)
	{
		perror("client:write server fifo error");
		exit(-1);
	}else{
		printf("write server fifo successfully.\n");
	}

	// close server.fifo
	close(sfd);

	// open client fifo in readonly mode
	if((cfd = open(clientfifo, O_RDONLY)) == -1)
	{
		perror("client:open client fifo error");
	}

	// read from client fifo and display in stdout
	while((rnum = read(cfd, buf, sizeof(buf))) > 0)
	{
		write(1, buf, rnum);
	}

	// close client fifo
	close(cfd);

	// delete client fifo
	unlink(clientfifo);
	
	return 0;
}
