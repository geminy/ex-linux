#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define FILE_NAME "testfile"
#define MAX_LEN (10)
#define INCRE_TIME (10)

int main(int argc, char *argv[])
{
    pid_t pid;
    int fd, i;
    char line[MAX_LEN + 1];
    ssize_t n;
    long seqno;

    pid = getpid();
    if ((fd = open(FILE_NAME, O_RDWR)) == -1) {
        handle_error_en(errno, "open");
    }
    for (i = 0; i < INCRE_TIME; ++i) {
        if (lseek(fd, 0, SEEK_SET) == -1) { // rewind before read
            handle_error_en(errno, "lseek");
        }
        if ((n = read(fd, line, MAX_LEN)) == -1) {
            handle_error_en(errno, "read");
        }
        line[n] = '\0'; // null ternimate for sscanf
        sscanf(line, "%ld\n", &seqno);
        printf("%s: pid = %ld, seq = %ld\n", argv[0], (long)pid, seqno);
        ++seqno; // increment sequence number
        snprintf(line, sizeof(line), "%ld\n", seqno);
        if (lseek(fd, 0, SEEK_SET) == -1) { // rewind before write
            handle_error_en(errno, "lseek");
        }
        if (write(fd, line, strlen(line)) == -1) {
            handle_error_en(errno, "write");
        }
    }

    if (close(fd) == -1) {
        handle_error_en(errno, "close");
    }
    exit(EXIT_SUCCESS);
}
