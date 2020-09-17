#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXLINE (8092)
#define FIFONAME "myfifo.f"
#define FIFOMODE (0644)

int main()
{
	char line[MAXLINE];
	int fifo;
	ssize_t nread;

	if (access(FIFONAME, F_OK) == -1) {
		perror("[read]");
		if (mkfifo(FIFONAME, FIFOMODE) == -1) {
			perror("[read]");
			return -1;
		}
	}

	if ((fifo = open(FIFONAME, O_RDONLY)) == -1) {
		perror("[read]");
		return -1;
	}

	while (1) {
		memset(line, 0, MAXLINE);
		nread = read(fifo, line, MAXLINE - 1);
		if (nread == -1) {
			perror("[read]");
			break;
		}
		else if (nread > 0) {
			printf("[read]:%s\n", line);
		}
	}

	close(fifo);
	unlink(FIFONAME);

	exit(0);
}