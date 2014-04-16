/* 测试管道最多能写入多少数据 */

#include  <stdio.h>
#include  <unistd.h>
#include  <stdlib.h>

int main(int argc, char *argv[])
{
	int p[2];
	int i;
	char buf[] = "a";
	
	if( pipe(p) == -1 )
	{
		perror("create pipe error");
		exit(-1);
	}

	for( i = 0 ;  ; i++ )
	{
		write(p[1], buf, sizeof(buf[0]));
		printf("write pipe %d, total %lu\n", i+1, (i+1)*sizeof(buf[0]));
	}

	return 0;
}
