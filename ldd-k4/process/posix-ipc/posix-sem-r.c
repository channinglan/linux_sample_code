#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdlib.h>           /* For O_* constants */
#include <string.h>
#include <semaphore.h>
#include <stdio.h>

struct shmbuf { /* Shared memory buffer */
	sem_t sem ; /* Semaphore to protect access */
	int seqnum ; /* Sequence number */
};

#define MAX_LEN 10000

struct shmbuf *rptr;
int fd;

int main(int argc, char **argv)
{
	int i = 0;
	
	if (argc != 2) {
		printf("invalid parameter\n");
		return -1;
	}

	/* Create shared memory object and set its size */
	fd = shm_open(argv[1], O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror("shm open failed\n"); /* Handle error */;
		return -1;
	}

	if (ftruncate(fd, sizeof(struct shmbuf)) == -1) {
		perror("ftruncate failed\n")	/* Handle error */;
		return -1;
	}

	/* Map shared memory object */
	rptr = mmap(NULL, sizeof(struct shmbuf),
			PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (rptr == MAP_FAILED) {
		perror("mmap failed\n") /* Handle error */;
		return -1;
	}

	while(1)
	{
		sem_wait(&rptr->sem);
		printf("%d\n",rptr->seqnum);
	}
}

