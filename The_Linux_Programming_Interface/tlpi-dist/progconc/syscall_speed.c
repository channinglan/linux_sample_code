/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2017.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Supplementary program for Chapter 3 */

/* syscall_speed.c

   By repeatedly invoking a simple system call (getppid()), we can get some
   idea of the cost of making system calls.

   Usage: time syscall_speed numcalls
                           Def=10000000

   Compiling with -DNOSYSCALL causes a call to a simple function
   returning an integer, which can be used to compare the overhead
   of a simple function call against that of a system call.
*/
#include "tlpi_hdr.h"
#include <sys/types.h>
#include <unistd.h>


#ifdef NOSYSCALL
static int myfunc() { return 1; }
#endif

int
main(int argc, char *argv[])
{
	int numCalls=2, j;
	pid_t  pid;
	pid_t  ppid;
	numCalls = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-calls") : 10;
        printf("numCalls %d\n",numCalls); 
#ifdef NOSYSCALL
        printf("Calling normal function\n");
#else
        printf("Calling getppid()\n");
#endif
	system("ps -aux ");
	for (j = 0; j < numCalls; j++) {
#ifdef NOSYSCALL
        	myfunc();
#else
		//pid_t getpid(void);
		//getpid() returns the process ID of the calling process. (This is often used by routines that generate unique temporary filenames.) 
        	pid = getppid();
        	//pid_t getppid(void);
        	//getppid() returns the process ID of the parent of the calling process. 
        	ppid = getppid();
        	printf("[%d]Calling getppid() parent %d pid %d\n",j,ppid,pid);       
#endif
	}
	exit(EXIT_SUCCESS);
}
