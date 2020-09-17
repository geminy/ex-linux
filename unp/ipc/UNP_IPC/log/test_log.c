#include <errno.h>

#define HAVE_VSNPRINTF
#include "log.h"

int main(int argc, char **argv)
{
    LOG_FUNC_BEGIN;

    err_msg("This is one %s", "error message");
    errno = 0;
    err_ret("This is one %s", "error return");
//    err_quit("This is one %s", "error quit");
//    errno = 1;
//    err_sys("This is one %s", "error system");
//    errno = 2;
//    err_dump("This is one %s", "error dump");

    LOG_FUNC_END;
    return 0;
}
