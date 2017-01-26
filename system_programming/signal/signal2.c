#include <stdio.h>  
#include <signal.h>  
#include <unistd.h>  
  
void show_handler(int sig)  
{  
    printf("I got signal %d\n", sig);  
    int i;  
    for(i = 0; i < 5; i++) {  
        printf("i = %d\n", i);  
        sleep(1);  
    }  
}  
   
int main(void)  
{  
    int i = 0;  
    struct sigaction act, oldact;  
    act.sa_handler = show_handler;  
    sigaddset(&act.sa_mask, SIGQUIT); //?ª`(1)  
    act.sa_flags = SA_RESETHAND | SA_NODEFER; //?ª`(2)  
    //act.sa_flags = 0; //?ª`(3)  
  
    sigaction(SIGINT, &act, &oldact);  
    while(1) {  
        sleep(1);  
        printf("sleeping %d\n", i);  
        i++;  
    }  
}  