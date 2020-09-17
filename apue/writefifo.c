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

	if (access(FIFONAME, F_OK) == -1) {
		perror("[write]");
		if (mkfifo(FIFONAME, FIFOMODE) == -1) {
			perror("[write]");
			return -1;
		}
	}

	if ((fifo = open(FIFONAME, O_WRONLY)) == -1) {
		perror("[write]");
		return -1;
	}

	while (1) {
		memset(line, 0, MAXLINE);
		fgets(line, MAXLINE, stdin);
		line[strlen(line) - 1] = 0;
		if (write(fifo, line, MAXLINE) == -1) {
			perror("[write]");
			break;
		}
		if (line[0] == 'q') {
			break;
		}
	}

	close(fifo);
	unlink(FIFONAME);

	exit(0);
}