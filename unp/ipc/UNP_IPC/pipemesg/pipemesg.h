#ifndef PIPEMESG
#define PIPEMESG

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef PIPE_BUF
#define PIPE_BUF (4096)
#endif

// Our own "messages" to use with pipes, FIFOS, and message queues.
// want sizeof(struct mymesg) <= PIPE_BUF
#define MAXMESGDATA (PIPE_BUF - 2 * sizeof(long))
// length of mesg_len and mesg_type
#define MESGHDRSIZE (sizeof(struct mymesg) - MAXMESGDATA)

struct mymesg
{
    long mesg_len; // bytes in mesg_data, can be 0
    long mesg_type; // message type, must be > 0
    char mesg_data[MAXMESGDATA];
};

ssize_t mesg_send(int fd, struct mymesg *mptr);
ssize_t mesg_recv(int fd, struct mymesg *mptr);

void client(int readfd, int writefd);
void server(int readfd, int writefd);

#endif // PIPEMESG
