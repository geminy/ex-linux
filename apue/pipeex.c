#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	int n;
	int fd[2];
	pid_t pid;
	char line[4096] = { 0 };

	if (pipe(fd) < 0) {
		printf("pipe error\n");
		return -1;
	}

	if ((pid = fork()) < 0) {
		printf("fork error");
		return -1;
	}
	else if (pid > 0) {
		close(fd[0]);
		write(fd[1], "pipe from parent\n", 17);
	}
	else {
		close(fd[1]);
		n = read(fd[0], line, 4096);
		write(STDOUT_FILENO, line, n);
	}

	exit(0);
}