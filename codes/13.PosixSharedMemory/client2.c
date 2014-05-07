// client 向缓冲区中放入msg
#include	"cliserv2.h"

int
main(int argc, char **argv)
{
	int		fd, i, nloop, nusec;
	pid_t	pid;
	char	mesg[MESGSIZE];
	long	offset;
	struct shmstruct	*ptr;

	if (argc != 4) {
		fprintf(stderr, "usage: client2 <name> <#loops> <#usec>");
		exit(1);
	}
	nloop = atoi(argv[2]);
	nusec = atoi(argv[3]);

		/* 4open and map shared memory that server must create */
	fd = shm_open((argv[1]), O_RDWR, FILE_MODE);
	ptr = mmap(NULL, sizeof(struct shmstruct), PROT_READ | PROT_WRITE,
			   MAP_SHARED, fd, 0);
	close(fd);

	pid = getpid();
	for (i = 0; i < nloop; i++) {
		sleep_us(nusec); 
		snprintf(mesg, MESGSIZE, "pid %ld: message %d", (long) pid, i);
/*
Our client follows the basic algorithm for the consumer but instead of calling
sem-wait (nempty),which is where the consumer blocks if there is no room in the
buffer for its message, we call sem-trywait, which will not block. If the value of the
semaphore is 0, an error of EAGAIN is returned. We detect this error and increment the
overflow counter.
*/
		if (sem_trywait(&ptr->nempty) == -1) {
			if (errno == EAGAIN) {
				sem_wait(&ptr->noverflowmutex);
				ptr->noverflow++;
				sem_post(&ptr->noverflowmutex);
				continue;
			} else {
				fprintf(stderr, "sem_trywait error");
				exit(1);
			}
		}
		sem_wait(&ptr->mutex);
		offset = ptr->msgoff[ptr->nput];
		if (++(ptr->nput) >= NMESG)
			ptr->nput = 0;		/* circular buffer */
		sem_post(&ptr->mutex);
		strcpy(&ptr->msgdata[offset], mesg);
		sem_post(&ptr->nstored);
	}
	exit(0);
}
