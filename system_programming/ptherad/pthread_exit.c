#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <pthread.h>                                   
#include <assert.h>                                                         
                                                         
void *thread_func(void*arg)                              
{                                                        
	pthread_exit(NULL);                              
	return NULL;                                     
}                                                        
                                                         
                                                         
int main()                                               
{                                                        
	int r;                                           
	pthread_t th;                                    
	r = pthread_create(&th, NULL, thread_func, NULL);
	assert(r == 0);                                  
	r = pthread_join(th, NULL);                      
	assert(r == 0);                                  
                                                         
                                                         
	return 0;                                        
}                                                        