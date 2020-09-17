#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <syslog.h>

// Print a message and return to caller.
// Caller specifies "errnoflag" and "level"
static void err_doit(int errnoflag, int level, const char*fmt, va_list ap)
{
    int errno_save, n;
    char buf[MAXLNE];
    errno_save = errno; // value caller might want printed
#ifdef HAVE_VSNPRINTF
    vsnprintf(buf, sizeof(buf), fmt, ap); // this is safe
#else
    vsprintf(buf, fmt, ap); // this is not safe
#endif
    n = strlen(buf);
    if (errnoflag) {
        snprintf(buf + n, sizeof(buf) - n, ": %s", strerror(errno_save));
    }
    strcat(buf, "\n");
    if (daemon_proc) {
        syslog(level, buf);
    }
    else {
        fflush(stdout); // in case stdout and stderr are the same
        fputs(buf, stderr);
        fflush(stderr);
    }
    return;
}

void daemon_init(int num)
{
    daemon_proc = num;
}

void err_ret(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(1, LOG_INFO, fmt, ap);
    va_end(ap);
    return;
}

void err_sys(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(1, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(1);
}

void err_dump(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(1, LOG_ERR, fmt, ap);
    va_end(ap);
    abort(); // dump core and terminate
    exit(1); // shouldn't get here
}

void err_msg(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(0, LOG_INFO, fmt, ap);
    va_end(ap);
    return;
}

void err_quit(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(0, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(1);
}
