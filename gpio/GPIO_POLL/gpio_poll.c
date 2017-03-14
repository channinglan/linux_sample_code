#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <poll.h>

//#include "vcs_gpio.h"
typedef struct {
    int fd;
    struct pollfd fds[1];
} vcs_gpio_params;
#define GPIO_TO_PIN(bank, gpio)      (32 * (bank-1) + (gpio))


int gpio_numbr = GPIO_TO_PIN(2,24);
int test_gpio_out = GPIO_TO_PIN(2,25);

int vcs_gpio_set(vcs_gpio_params *gpio_params, char *value)
{
    int status = 0;

    if (gpio_params == NULL) {
        printf("in %s: parameter error\n", __func__);
        return -1;
    }

    return status;
}

/*
 * this api get gpio(SYS_IN) value
 * using lseek to select the begins of the file
 * and then read just one charater
 */
int vcs_gpio_get(vcs_gpio_params *gpio_params, char *value)
{           
    int status = 0;

    if (gpio_params == NULL) {
        printf("in %s: parameter error\n", __func__);
        return -1;
    }

    status = lseek(gpio_params->fd, 0, SEEK_SET);
    if (status == -1) {
        printf("in %s: lseek gpio error\n", __func__);
        return -1;
    }

    status = read(gpio_params->fd, value, 1);
    if (status == -1) {
        printf("in %s: read  gpio error\n", __func__);
        return -1;
    }
 
    return status;
}

int vcs_gpio_create(vcs_gpio_params *gpio_params)
{
    int status = 0;
	char str[256];
    if (gpio_params == NULL) {
        printf("in %s: parameter error\n", __func__);
        return -1;
    }

    //using falling to trigger interrupt
	sprintf(str,"echo %d > /sys/class/gpio/export",gpio_numbr);
	system(str);
	
 //   system("echo 57 > /sys/class/gpio/export");
	sprintf(str,"echo falling > /sys/class/gpio/gpio%d/edge",gpio_numbr);
	system(str);
//	system("echo falling > /sys/class/gpio/gpio56/edge");
    
 	sprintf(str,"/sys/class/gpio/gpio%d/value",gpio_numbr);
    gpio_params->fd = open(str, O_RDONLY);
 //   gpio_params->fd = open("/sys/class/gpio/gpio56/value", O_RDONLY);
 
	//test output
	sprintf(str,"echo %d > /sys/class/gpio/export",test_gpio_out);
	system(str);
	sprintf(str,"echo out > /sys/class/gpio/gpio%d/direction",test_gpio_out);
	system(str);
	sprintf(str,"echo 0 > /sys/class/gpio/gpio%d/value",test_gpio_out);
	system(str);	
		   
    
    if (gpio_params->fd == -1) {
        printf("in %s: open gpio error\n", __func__);
        return -1;
    }

    gpio_params->fds[0].fd     = gpio_params->fd;
    gpio_params->fds[0].events = POLLPRI;

    return status;
}

int vcs_gpio_delete(vcs_gpio_params *gpio_params)
{
    int status = 0;

    if (gpio_params == NULL) {
        printf("in %s: parameter error\n", __func__);
        return -1;
    }
    
    gpio_params->fds[0].fd     = -1;
    gpio_params->fds[0].events = 0x00;

    if (gpio_params->fd <= 0)
        return 0;

    return close(gpio_params->fd);
}

/*
 * this api poll the gpio's interrupt.
 * if the poll function return then the button is pressed
 * it have three situations. 
 * one is a jitter
 * the second situation is a reset event
 * the third situation is a vcs_system_update_or_reload event.
 */
int vcs_gpio_service(vcs_gpio_params *gpio_params)
{
    char val;
    int status = 0;

    if (gpio_params == NULL) {
        printf("in %s: parameter error\n", __func__);
        return -1;
    }

    status = vcs_gpio_get(gpio_params, &val);
    if (status == -1) {
        printf("in %s: gpio get error\n", __func__);
        return -1;
    }

    while (1) {
        status = poll(gpio_params->fds, 1, -1);
        if (status == -1) {
            printf("in %s: poll gpio error\n", __func__);
            return -1;
        }

        if (gpio_params->fds[0].revents & POLLPRI) {
            
            usleep(50 * 1000);
            status = vcs_gpio_get(gpio_params, &val);
            if (status == -1) {
                printf("in %s: gpio get error\n", __func__);
                return -1;
            }
            if (val == '1') {
                printf("in %s: gpio is jitter\n", __func__);
                continue;
            }
            
            usleep(150 * 1000);
            status = vcs_gpio_get(gpio_params, &val);
            if (status == -1) {
                printf("in %s: gpio get error\n", __func__);
                return -1;
            }
            if (val == '1') {
                printf("in %s: gpio get reset\n", __func__);
                system("echo out > /sys/class/gpio/gpio57/direction");
                continue;
            }
            
            usleep(150 * 1000);
            status = vcs_gpio_get(gpio_params, &val);
            if (status == -1) {
                printf("in %s: gpio get error\n", __func__);
                return -1;
            }
            if (val == '1') {
                printf("in %s: gpio get reset\n", __func__);
                system("echo out > /sys/class/gpio/gpio57/direction");
                continue;
            }
        }
    }

    return status;
}

int main(void)
{
    int status = 0;
    vcs_gpio_params gpio_params;

    memset(&gpio_params, 0x00, sizeof(vcs_gpio_params));
    status = vcs_gpio_create(&gpio_params);
    if (status == -1) {
        printf("vcs_gpio_create error\n");
        return -1;
    }

    status = vcs_gpio_service(&gpio_params);
    if (status == -1) {
        printf("vcs_gpio_service error\n");
        return -1;
    }

    status = vcs_gpio_delete(&gpio_params);
    if (status == -1) {
        printf("vcs_gpio_delete error\n");
        return -1;
    }

    return 0;
}

