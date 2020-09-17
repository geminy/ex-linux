#include "pipemesg.h"

void server(int readfd, int writefd)
{
    FILE *fp;
    ssize_t n;
    struct mymesg mesg;
    // read pathname from IPC channel
    mesg.mesg_type = 1;
    if (0 == (n = mesg_recv(readfd, &mesg))) {
        printf("pathname missing\n");
        return;
    }

    mesg.mesg_data[n] = '\0'; // null terminate pathname
    if (NULL == (fp = fopen(mesg.mesg_data, "r"))) {
        // error: must tell client
        snprintf(mesg.mesg_data + n, sizeof(mesg.mesg_data) - n, ":can't open, %s\n", strerror(errno));
        mesg.mesg_len = strlen(mesg.mesg_data);
        mesg_send(writefd, &mesg);
    }
    else {
        // fopen succeeded: copy file to IPC channel
        while (NULL != (fgets(mesg.mesg_data, MAXMESGDATA, fp))) {
            mesg.mesg_len = strlen(mesg.mesg_data);
            mesg_send(writefd, &mesg);
        }
        fclose(fp);
    }

    // send a 0-length message to signify the end
    mesg.mesg_len = 0;
    mesg_send(writefd, &mesg);
}
