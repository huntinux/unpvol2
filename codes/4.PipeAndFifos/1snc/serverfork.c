/* 
   不断处理cilent的请求, 使用fork，属于并发服务 
   首先从server读取client传送的pid，pathname
   然后带开pathname 和 client fifo
   将pathname的内容读出，写入到client fifo中。

problem: 不能持续处理client请求，有时从server fifo
读出的数据个数为0 导致server退出。
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
	pid_t cpid, scpid;
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

	while(1)
	{
		if((rnum = read(sfd, buf, sizeof(buf))) > 0)
		{
			printf("server: read:%s\n", buf);

			if((scpid = fork()) == 0) { // subprocess: process the request

				sscanf(buf, "%d %s", &cpid, pathname);
				printf("server: get client pid: %d,pathname:%s\n", cpid, pathname);

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

				// close client fifo, pathname and exit
				close(fd);
				close(cfd);
				exit(0);

			}else if(scpid > 0) { // mainprocess

			}
		}
	}
	printf("rnum=%d\n", rnum);

	// close server fifo
	close(sfd);

	// wait
	wait(NULL);

	// delete server fifo
	unlink(serverfifo);

	return 0;
}





