#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <strings.h>

#define PORT (13)
#define MAXLINE (50)

int main(int argc, char **argv)
{
	if (2 != argc) {
		printf("usage: %s <IPAddress>\n", argv[0]);
		exit(0);
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		perror("socket error");
		exit(0);
	}

	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if (1 != inet_pton(AF_INET, argv[1], &servaddr.sin_addr)) {
		printf("inet_pton error: %s\n", argv[1]);
		close(sockfd);
		exit(0);
	}

	if (0 != connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) {
		perror("connect error");
		close(sockfd);
		exit(0);
	}

	int n;
	char recvline[MAXLINE + 1];
	while (0 < (n = read(sockfd, recvline, MAXLINE))) {
		recvline[n] = 0;
		if (EOF == fputs(recvline, stdout)) {
			printf("fputs error\n");
			close(sockfd);
			exit(0);
		}
	}
	if (0 > n) {
		printf("read error: %s\n", strerror(errno));
	}

	close(sockfd);
	exit(0);
}
