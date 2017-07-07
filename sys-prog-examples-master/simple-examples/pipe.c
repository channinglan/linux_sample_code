// Scott Kuhl
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1



int read_pipe(int *fd,int len)
{
	char buf[256];
	
	printf("\nead_pipe fd %p,led %d\n", fd,len);
	
	ssize_t readRetVal = read(fd[READ_END], &buf, len);
	if(readRetVal == 0)
	{
		printf("Read end of file from pipe\n");
		//exit(EXIT_SUCCESS);
		return 0;
	}
	else if(readRetVal == -1)
	{
		perror("write");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Read character %s\n", buf);
	}	
	return 0;
	
}

int main(void)
{
	int fd[2];	
	if(pipe(fd) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	if(write(fd[WRITE_END], "hello", 5) == -1)
	{
		perror("write");
		exit(EXIT_FAILURE);
	}
	close(fd[WRITE_END]); // TRY THIS: What happens if we don't close the write end?
	
	while(1)
	{
		char buf;
		ssize_t readRetVal = read(fd[READ_END], &buf, 1);
		if(readRetVal == 0)
		{
			printf("Read end of file from pipe\n");
			//exit(EXIT_SUCCESS);
			break;
		}
		else if(readRetVal == -1)
		{
			perror("write");
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("Read character %c\n", buf);
		}
	}
	

	if(pipe(fd) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	if(write(fd[WRITE_END], "test ", 5) == -1)
	{
		perror("write");
		exit(EXIT_FAILURE);
	}
	close(fd[WRITE_END]); // TRY THIS: What happens if we don't close the write end?	
	
	read_pipe(fd,sizeof("test"));
	
	
	
}
