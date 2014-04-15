#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    int fd;
    char buf[512];
    int rnum;

    rnum = read(0, buf, 512); // open will store '\n'
    buf[rnum-1]='\0';
    printf("%s\n", buf);

    fd = open(buf, O_RDONLY);
    if(fd < 0){
        perror("open error");
    }else{

        printf("open success");
    }

    close(fd);

    return 0;
}
