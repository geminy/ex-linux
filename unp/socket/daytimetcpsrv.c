#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <time.h>

#define PORT (13)
#define MAXLINE (50)
#define LISTENQ (5)

int main(int argc, char **argv)
{
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == listenfd) {
		printf("socket error: %s", strerror(errno));
		exit(0);
	}

	int reuse = 1;
	if (0 > (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)))) {
		printf("setsockopt error: %s\n", strerror(errno));
	}

	struct  sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	if (-1 == bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) {
		perror("bind error");
		close(listenfd);
		exit(0);
	}
	
	if (-1 == listen(listenfd, LISTENQ)) {
		perror("listen error");
		close(listenfd);
		exit(0);
	}

	int connfd;
	time_t ticks;
	char buff[MAXLINE];
	for(;;) {
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
		if (-1 == connfd) {
			perror("accept error");
			exit(0);
		}
		
		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%s", ctime(&ticks));
		write(connfd, buff, strlen(buff));
		close(connfd);
	}

	close(listenfd);
	exit(0);
}
