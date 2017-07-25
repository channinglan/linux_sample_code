#define _GNU_SOURCE 
#include <sys/time.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <dlfcn.h> 
#include <pthread.h> 

static void show_stackframe() {
	void *trace[16];
	char **messages = (char **)NULL;
	int i, trace_size = 0;

	trace_size = backtrace(trace, 16);
	messages = backtrace_symbols(trace, trace_size);
	printf("[bt] Execution path:\n");
	for (i=0; i < trace_size; ++i)
		printf("[bt] %s\n", messages[i]);
}

/* Original pthread function */ 
static int (*pthread_create_orig)(pthread_t *__restrict, 
		__const pthread_attr_t *__restrict, 
		void *(*)(void *), 
		void *__restrict); 

/* Library initialization function */ 
void helperinit(void) __attribute__((constructor)); 

void helperinit(void) 
{ 
	pthread_create_orig = dlsym(RTLD_NEXT, "pthread_create"); 
	if(pthread_create_orig == NULL) 
	{ 
		char *error = dlerror(); 
		if(error == NULL) 
		{ 
			error = "pthread_create is NULL"; 
		} 
		fprintf(stderr, "%s\n", error); 
		exit(EXIT_FAILURE); 
	} 
} 

/* Our wrapper function for the real pthread_create() */ 
int pthread_create(pthread_t *__restrict thread, 
		__const pthread_attr_t *__restrict attr, 
		void * (*start_routine)(void *), 
		void *__restrict arg) 
{ 
	int ret; 

	printf("pthread creating......\n");
	show_stackframe();

	/* The real pthread_create call */ 
	ret = pthread_create_orig(thread, 
			attr, 
			start_routine, 
			arg); 
	return ret; 
}
