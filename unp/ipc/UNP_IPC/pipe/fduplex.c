#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int fd[2];
    int n;
    char c;
    pid_t childpid;

    pipe(fd);

    if (0 == (childpid = fork())) {
        sleep(3);
        if (-1 == (n = read(fd[0], &c, 1))) {
            printf("child: read error, returnd %d, %s\n", n, strerror(errno));
        }
        else {
            printf("child: read ok, returnd %c\n", c);
        }

        if (-1 == write(fd[0], "c", 1))
        {
            printf("child: write error, %s\n", n, strerror(errno));
        }
        exit(EXIT_SUCCESS);
    }

    if (-1 == write(fd[1], "p", 1))
    {
        printf("parent: write error, %s\n", strerror(errno));
    }

    if (-1 == (n = read(fd[1], &c, 1))) {
        printf("parent: read error, returnd %d, %s\n", n, strerror(errno));
    }
    else {
        printf("parent: read ok, returnd %c\n", c);
    }

    exit(EXIT_SUCCESS);
}
