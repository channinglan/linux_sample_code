#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <signal.h>
#include <stdio.h>

#define STACK_SIZE (1024 * 1024)

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

static char child_stack[STACK_SIZE];

static void set_map(char* file, int inside_id, int outside_id)
{
	FILE* mapfd = fopen(file, "w");
	if (NULL == mapfd) {
		perror("open file error");
		return;
	}
	fprintf(mapfd, "%d %d %d", inside_id, outside_id, 1);
	fclose(mapfd);
}

static void set_uid_map(pid_t pid, int inside_id, int outside_id)
{
	char file[256];
	sprintf(file, "/proc/%d/uid_map", pid);
	set_map(file, inside_id, outside_id);
}

static void set_gid_map(pid_t pid, int inside_id, int outside_id)
{
	char file[256];
	sprintf(file, "/proc/%d/gid_map", pid);
	set_map(file, inside_id, outside_id);
}

static int child_main(void *arg)
{
	sleep(1); //wait for 1 second to make certain uid_map and gid_map is written
	printf("child\n");
	system("mount -t proc none /proc");
	mount("/home/baohua/develop/training/namespace/test-dir", "/mnt", "none", MS_BIND, NULL);
	sethostname("container",10);
	execlp("/bin/bash","bash",NULL,NULL);
	return 1;
}

int main()
{
	pid_t child_pid;

	child_pid = clone(child_main,child_stack+STACK_SIZE,SIGCHLD | CLONE_NEWPID |
			CLONE_NEWNS | CLONE_NEWNET | CLONE_NEWUTS | CLONE_NEWUSER, NULL);
	if (child_pid == -1)
		errExit("clone");

	set_uid_map(child_pid, 0, getuid());
	set_gid_map(child_pid, 0, getgid());

	wait(NULL);

	return 0;
}
