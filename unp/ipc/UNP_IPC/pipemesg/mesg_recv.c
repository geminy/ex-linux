#include "pipemesg.h"

ssize_t mesg_recv(int fd, struct mymesg *mptr)
{
    size_t len;
    ssize_t n;
    // read massage header first, to get len of data that follows
    if (0 == (n = read(fd, mptr, MESGHDRSIZE))) {
        return 0; // end of file
    }
    else if (MESGHDRSIZE != n) {
        printf("message header expected %d, got %d\n", MESGHDRSIZE, n);
        return -1;
    }

    if (0 < (len = mptr->mesg_len)) {
        if (len != (n = read(fd, mptr->mesg_data, len))) {
            printf("message header expected %d, got %d\n", len, n);
            return -1;
        }
    }

    return len;
}
