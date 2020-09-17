#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
	int fdin;
	int fdout;
	void *src;
	void *dst;
	struct stat statbuf;

	if (3 != argc) {
		printf("usage: %s <fromfile> <tofile>\n", argv[0]);
		return -1;
	}

	if ((fdin = open(argv[1], O_RDONLY)) < 0) {
		printf("can't create %s for reading\n", argv[1]);
		return -1;
	}

	if ((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC)) < 0) {
		printf("can't create %s for writing\n", argv[2]);
		return -1;
	}

	if (fstat(fdin, &statbuf) < 0) {
		printf("fstat error\n");
		return -1;
	}

	if (lseek(fdout, statbuf.st_size - 1, SEEK_SET) == -1) {
		printf("lseek error\n");
		return -1;
	}

	if (write(fdout, "", 1) != 1) {
		printf("write error\n");
		return -1;
	}

	if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == MAP_FAILED) {
		printf("mmap error for input\n");
		return -1;
	}

	if ((dst = mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0)) == MAP_FAILED) {
		printf("mmap error for output\n");
		return -1;
	}

	memcpy(dst, src, statbuf.st_size);

	// TODO: close munmap

	exit(0);
}
