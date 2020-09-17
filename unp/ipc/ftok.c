#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if (2 != argc) {
		printf("usage: %s <pathname>\n", argv[0]);
		exit(0);
	}

	struct stat st;
	if (0 != stat(argv[1], &st)) {
		perror("stat error");
		exit(0);
	}

	key_t key = ftok(argv[1], 0x57);
	if (-1 == key) {
		printf("ftok error: %s\n", strerror(errno));
		exit(0);
	}

	printf("st_dev: %lx, st_ino: %lx, key: %x\n",
			(unsigned long)st.st_dev, (unsigned long)st.st_ino, key);

	exit(0);
}
