/* 
	只处理一个cilent的请求，完成后退出 
	首先从server读取client传送的pid，pathname
	然后带开pathname 和 client fifo
	将pathname的内容读出，写入到client fifo中。
 */

#include  <stdio.h>
#include  <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const char * serverfifo = "/tmp/server.fifo";

int main()
{
	char buf[64];
	pid_t cpid;
	int sfd, cfd, fd;
	int rnum;
	char pathname[64];

	// make server fifo 
	if(mkfifo(serverfifo, O_CREAT|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) == -1)
	{
		perror("server mkfifo error");
		//exit(-1);
	}else{
		printf("create server fifo successfully.\n");
	}

	// open server fifo and read pid of client, pathname from it
	if((sfd = open(serverfifo, O_RDONLY)) == -1)
	{
		perror("server open server fifo error");
		exit(-1);
	}else{
		printf("open server fifo in readonly mode successfully.\n");
	}

	if((rnum = read(sfd, buf, sizeof(buf))) == -1)
	{
		perror("server read server fifo error");
		exit(-1);
	}else{
		printf("server: read:%s\n", buf);
	}

	sscanf(buf, "%d %s", &cpid, pathname);
	printf("server: get client pid: %d,pathname:%s\n", cpid, pathname);
	// close server fifo
	close(sfd);

	// open client fifo: writeonly
	snprintf(buf, sizeof(buf), "/tmp/client.%d.fifo", cpid);
	printf("server:client fifo is %s\n", buf);
	if((cfd = open(buf, O_WRONLY)) == -1)
	{
		perror("server:open client fifo error");
		exit(-1);
	}else{
		printf("open client fifo for writeonly\n");
	}

	// open file:pathname, read it, and write its content to client fifo
	if((fd = open(pathname, O_RDONLY)) == -1)
	{
		perror("open pathname error.");
		exit(-1);
	}else{
		printf("open pathname successfully.\n");
	}

	while((rnum = read(fd, buf, sizeof(buf))) > 0)
	{
		write(cfd, buf, rnum);
	}

	// close client fifo, pathname
	close(fd);
	close(cfd);

	// delete server fifo
	unlink(serverfifo);

	return 0;
}
