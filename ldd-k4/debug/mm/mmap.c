#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	volatile unsigned char *m;
	unsigned int i;
	/* 
	 * important notes:
	 * this needs sudo permission
	 */
	int fd = open("/dev/sda", O_RDONLY, S_IRUSR);
	if (fd == -1) {
		perror("/dev/sda open() failed, use 'sudo'\n");
		exit(-1);
	};

#define MAP_SIZE 2048UL * 1024 * 1024
	m = mmap(0, MAP_SIZE, PROT_READ | PROT_EXEC,
			MAP_PRIVATE, fd, 0);

	if (m == MAP_FAILED) {
		perror("mmap /dev/sda failed\n");
		exit(-1);
	}

	sleep(1);
	printf("memory base:%p\n", m);

	for (i = 0 ; i < MAP_SIZE; i++) {
		(void)m[i];
		if( (i & 0xfffff) == 0) {
			usleep(100000);
			printf("MiB:%d\n", i >> 20);
			/*
			 * enable this will trigger OOM killer(don't release mmap area)
			 * disable this will trigger Page Reclaim(no OOM)
			 */
#if 0
			mlock(m, i);
#endif
		}
	}

	pause();
}
