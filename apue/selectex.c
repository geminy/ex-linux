#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	fd_set rfds;
	struct timeval tv;
	int retval;
	char buf[1024] = { 0 };

	FD_ZERO(&rfds);
	FD_SET(0, &rfds);

	tv.tv_sec = 5;
	tv.tv_usec = 0;

	retval = select(1, &rfds, NULL, NULL, &tv);

	if (retval == -1)
		perror("select()");
	else if (retval) {
		printf("Data is available now.\n");
		printf("FD_ISSET(0, fd_set) is %d\n", FD_ISSET(0, &rfds));
		read(0, buf, 1024);
		printf("buf = %s\n", buf);
	}  else
	printf("No data within five seconds.\n");

	exit(EXIT_SUCCESS);
}