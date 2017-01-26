#include <signal.h>
#include <stdio.h>
#include <unistd.h>


//SIG_DFL - Restore default behavior
//SIG_IGN - Ignore the signal

void caught(int sig)
{
	printf("\n I got signal %d \n",sig);
	(void) signal(SIGINT,SIG_DFL);
	(void) signal(SIGTERM,SIG_DFL);	
}

int main()
{
	(void) signal(SIGINT,caught);
	(void) signal(SIGTERM,caught);		
	while(1) {
		printf("\n Hello");
		sleep(1);	
		
		
	}
	
}
