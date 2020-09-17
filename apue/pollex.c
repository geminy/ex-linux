#include <stdio.h>
#include <stdlib.h>
#include <poll.h>

int main()
{
	struct pollfd pfd;
	nfds_t nfds;
	int timeout;
	int retval;
	char buf[1024] = { 0 };

	pfd.fd = 0;
	pfd.events = POLLIN;
	nfds = 1;
	timeout  = 5000;

	retval = poll(&pfd, nfds, timeout);

	if (retval == -1)
		perror("poll()");
	else if (retval) {
		printf("Data is available now.\n");
		printf("pollfd.revents & POLLIN = %d\n", pfd.revents & POLLIN);
		read(0, buf, 1024);
		printf("buf = %s\n", buf);
    }  else {
		printf("No data within five seconds.\n");
    }

	exit(EXIT_SUCCESS);
}
