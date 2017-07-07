// Scott Kuhl
//
// poll() lets you wait for up to a certain amount of time for a file
// descriptor to become ready for some kind of IO. It is similar to
// select().
//
// ppoll() provides similar functionality to poll(), but provides
// finer granularity and can mask signals that might interrupt the
// function (if poll() or ppoll() is interrupted by a signal, they
// return -1 and set errno to EINTR.
//
// Also see epoll().
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>

int main(void)
{
	printf("Wait 5 seconds for poll() to time out, or press a key to cause it to return before it times out.\n");

	struct pollfd fds;
	fds.fd = STDIN_FILENO;
	fds.events = POLLIN; // return when there is data to read

	// Arguments to poll()
	//
	// 1st argument is an array of fds (or in this case, an array of
	// one item!)
	//
	// 2nd argument is the number of items in the array.
	//
	// 3rd argument is time to wait in milliseconds
	int retval = poll(&fds, 1, 5000); 
		
	if (retval == -1)
		perror("poll()");
	else if (retval == 0)
		printf("Timed out. No data within five seconds.\n");
	else
	{
		printf("Data is available now.\n");

		// You can get more information from fds.revents---which
		// poll() uses to output information. For example, you can
		// trigger this by calling close(STDIN_FILENO) prior to
		// calling poll.
		if(fds.revents & POLLNVAL)
		{
			printf("file descriptor was bad\n");
		}
	}



	/* Try to read the bytes from stdin using read(). If we don't do
	 * this, the message that the user typed might instead get sent to
	 * the shell!
	 */
	if(1) // TRY THIS: Change this to 0!
	{
		char buf[1024];
		read(STDIN_FILENO, buf, 1024);
	}

	
	exit(EXIT_SUCCESS);
}
