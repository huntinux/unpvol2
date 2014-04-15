#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>

int main(int argc, char* argv[])
{
    struct stat s;

    if(argc != 2){
        fprintf(stderr, "usage: ftok <pathname>\n");
        exit(-1);
    }
    
    if(stat(argv[1], &s) == -1){
        perror("stat error");
        exit(-1);
    }
    
    printf("st_dev: %lx, st_ino: %lx, key: %x\n", 
        (unsigned long)s.st_dev, (unsigned long)s.st_ino,
        ftok(argv[1], 0x57));

    return 0;
}


