/*
    client 从stdin都入filepath. 然后通过管道1 发送给 server
    server  通过管道1，获得filepath，然后打开该文件，把文件内容
    通过管道2传给client
    client通过管道2将文件内容读出。
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
    int p1[2];
    int p2[2];

    if(pipe(p1) == -1){
        perror("pipe error");
        exit(-1);
    }   

    if(pipe(p2) == -1){
        perror("pipe error");
        exit(-1);
    }   
 
    p = fork();
    if(p < 0){
        perror("fork error");
        exit(-1);

    }else if(p > 0){ // client 
        close(p1[0]);
        close(p2[1]);

        char buf[512];
        int rnum;

        if((rnum = read(0, buf, 512)) == 0){
            write(2, "reach EOF\n", 10);
            exit(-1);
        }
        write(p1[1], buf, rnum);

        while((rnum = read(p2[0], buf, 512)) > 0){
            write(1, buf, rnum);
        }
        exit(0);

    }else{ // server 
        close(p1[1]);
        close(p2[0]);

        int rnum;
        char buf[512];
        int fd;

        rnum = read(p1[0], buf, 512);
        buf[rnum-1] = '\0';
         
        if((fd = open(buf, O_RDONLY)) < 0){
            write(p2[1], "openerror\n", 11);
        }else {
            while((rnum = read(fd, buf, 512)) > 0){
                write(p2[1], buf, rnum);
            }
            close(fd);
        }

        waitpid(p, NULL, 0);
    }     

    return 0;
}
