/* sscanf 使用示例 */

#include <stdio.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	char msg[] = "2314 ./client.c";
	pid_t cpid;
	char pathname[64];

	sscanf(msg, "%d %s", &cpid, pathname);
	printf("cpid:%d\npathname:%s\n", cpid, pathname);

	return 0;
}
