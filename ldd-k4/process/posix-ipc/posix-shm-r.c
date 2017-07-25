#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdlib.h>           /* For O_* constants */
#include <string.h>
#include <stdio.h>

#define MAX_LEN 10000
struct region {        /* Defines "structure" of shared memory */
	int len;
	char buf[MAX_LEN];
};
struct region *rptr;
int fd;

int main(int argc, char **argv)
{
	/* Create shared memory object and set its size */
	fd = shm_open(argv[1], O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror("shm open failed\n"); /* Handle error */;
		return -1;
	}

	if (ftruncate(fd, sizeof(struct region)) == -1) {
		perror("ftruncate failed\n")	/* Handle error */;
		return -1;
	}

	/* Map shared memory object */
	rptr = mmap(NULL, sizeof(struct region),
			PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (rptr == MAP_FAILED) {
		perror("mmap failed\n") /* Handle error */;
		return -1;
	}

	/* Now we can refer to mapped region using fields of rptr;
	   for example, rptr->len */
	printf("%d %s\n", rptr->len, rptr->buf);
}

