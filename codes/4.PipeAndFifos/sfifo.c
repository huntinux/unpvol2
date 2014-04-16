/*
    client 从stdin都入filepath. 然后通过fifo1 发送给 server
    server  通过fifo1，获得filepath，然后打开该文件，把文件内容
    通过fifo2传给client
    client通过fifo2将文件内容读出。
*/
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
    pid_t p; 
	int f1, f2;

    if(mkfifo("./fifo1", O_CREAT | S_IRUSR | S_IWUSR) == -1){
        perror("mkfifo error");
        exit(-1);
    }   
	if( (f1 = open("./fifo1", O_RDWR)) == -1)
	{
        perror("open fifo error");
        exit(-1);
	}

    if(mkfifo("./fifo2", O_CREAT | S_IRUSR | S_IWUSR) == -1){
        perror("mkfifo error");
        exit(-1);
    }   
	if( (f2 = open("./fifo2", O_RDWR)) == -1)
	{
        perror("open fifo error");
        exit(-1);
	}
 
    p = fork();
    if(p < 0){
        perror("fork error");
        exit(-1);

    }else if(p > 0){ // client 

        char buf[512];
        int rnum;

        if((rnum = read(0, buf, 512)) == 0){
            write(2, "reach EOF\n", 10);
            exit(-1);
        }
        write(f1, buf, rnum);

        while((rnum = read(f2, buf, 512)) > 0){
            write(1, buf, rnum);
        }
        exit(0);

    }else{ // server 

        int rnum;
        char buf[512];
        int fd;

        rnum = read(f1, buf, 512);
        buf[rnum-1] = '\0';
         
        if((fd = open(buf, O_RDONLY)) < 0){
            write(f2, "openerror\n", 11);
        }else {
            while((rnum = read(fd, buf, 512)) > 0){
                write(f2, buf, rnum);
            }
            close(fd);
        }

        waitpid(p, NULL, 0);
    }     

    return 0;
}
