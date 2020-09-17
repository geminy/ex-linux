#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE (100) // buffer size

int main(int argc, char **argv)
{
    size_t n;
    char buff[MAXLINE];
    char command[MAXLINE];
    FILE *fp;

    fgets(buff, MAXLINE, stdin); // get file name
    n = strlen(buff);
    if ('\n' == buff[n - 1]) {
        --n; // delete newline from fgets
    }
    snprintf(command, sizeof(command), "cat %s", buff); // shell command

    if (NULL == (fp = popen(command, "r"))) { // create pipe for read
        perror("popen error");
        exit(EXIT_FAILURE);
    }

    while(NULL != fgets(buff, MAXLINE, fp)) {
        fputs(buff, stdout);
    }

    pclose(fp); // close from popen

    exit(EXIT_SUCCESS);
}
