#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "gpio.h"

void listitem() {
	printf("1.  TEST SIG\n");
	printf("99. Exit Program\n");
	printf("Please enter the choice>> ");
}


void help(void) {
	printf("\n");
	printf("====================================================================================\n");
}

void receiveData(int n, siginfo_t *info, void *unused)
{
	system("echo 1 > /sys/class/leds/powerLED:red/brightness");
	printf("received value %i\n", info->si_int);
	usleep(20000);	
	system("echo 0 > /sys/class/leds/powerLED:red/brightness");	
}


int main(int argc, char *argv[])
{
	//gpio_params gpio;
	unsigned int cmd;
	int fd;
	int pid,value;

	int configfd;
	char buf[10];
	/* setup the signal handler for SIG_TEST 
 	 * SA_SIGINFO -> we want the signal handler function with 3 arguments
 	 */
	struct sigaction sig;

		
	/* open the /dev/icpgpio dev file */
	fd = open("/dev/gpio_int", O_RDONLY);
	if(fd < 0) {
		printf("Can not open /dev/gpio_int\n");
		return -1;
	}


	if (argc == 1) {
	
	} else if ((argc ==2) || (argc > 3)){
		help();
		return 0;
	} 



	sig.sa_sigaction = receiveData;
	sig.sa_flags = SA_SIGINFO;
	sigaction(SIG_TEST, &sig, NULL);
	
	//set pid to driver
	pid = getpid();
	ioctl(fd, GPIO_SET_PID, &pid);
	system("echo 0 > /sys/class/leds/powerLED:red/brightness");
	while(1) {
		listitem();
		if (scanf("%d",&cmd)==0) {
			scanf("%*[\n]");
			getchar();
			continue;
		}

		switch (cmd) {
		case 1:
			ioctl(fd, GPIO_SEND_SIG_TEST,NULL);
			break;
				
		case 99:
			close(fd);
			exit(0);
			break;
		default:
			break;
		}
	}

	return 0;
}
