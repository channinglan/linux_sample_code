#include <stdio.h>
#include <sched.h>
#include <signal.h>

void sigint_handle(int sig)
{
	printf("<pid:%d>got the %d signal...\n",getpid(), sig);
	exit(0);
}

int main(int argc, char* argv[])
{
	int pid1,pid2,pid3,ch=0;
	if(signal(SIGINT, sigint_handle) == SIG_ERR){
		perror("cannot reset the SIGINT signal handler");
		return 1;
	}

	pid1 = fork();
	printf("<pid:%d>pid1=%d\n", getpid(),pid1);	
	if(pid1==0) {
			ch+=1;
	}
	pid2 = fork();
	printf("<pid:%d>pid2=%d\n", getpid(),pid2);
	if(pid2==0) {
			ch+=2;
	}
	pid3 = fork();
	printf("<pid:%d>pid3=%d\n", getpid(),pid3);
	if(pid3==0) {
			ch+=4;
	}
	
	printf("<pid:%d><ch=%d>hello world\n",getpid(),ch);
	while(1);
}
