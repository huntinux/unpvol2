
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

	// read pid of client, pathname from server.fifo
	if(mkfifo(serverfifo, O_CREAT|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) == -1)
	{
		perror("server mkfifo error");
		//exit(-1);
	}
	if((sfd = open(serverfifo, O_RDONLY)) == -1)
	{
		perror("server open server fifo error");
		exit(-1);
	}
	if((rnum = read(sfd, buf, sizeof(buf))) == -1)
	{
		perror("server read server fifo error");
		exit(-1);
	}
	printf("heeee");
	sscanf(buf, "%d %s", cpid, pathname);
	printf("xx");
	printf("server: get client pid: %d\npathname:%s\n", cpid, pathname);

	// create client fifo, and open it: writeonly
	snprintf(buf, sizeof(buf), "/tmp/client.%d.fifo", cpid);
	printf("server:client fifo is %s\n", buf);
	if((cfd = open(buf, O_WRONLY)) == -1)
	{
		perror("server:open client fifo error");
		exit(-1);
	}

	// open file:pathname, read it, and write its content to client fifo
	fd = open(pathname, O_RDONLY);
	while((rnum = read(fd, buf, sizeof(buf)) > 0))
	{
		write(cfd, buf, rnum);
	}

	return 0;
}
