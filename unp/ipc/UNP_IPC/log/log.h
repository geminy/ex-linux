#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>

#define HAVE_VSNPRINTF

#define MAXLNE (4096) // max text line length

/*
 * @function  @strerror(errno)?  @called  @syslog
 * err_dump                y                  abort();  LOG_ERR
 * err_msg                   n                 return;   LOG_INFO
 * err_quit                   n                  exit(1);   LOG_ERR
 * err_ret                     y                  return;   LOG_INFO
 * err_sys                    y                  exit(1);    LOG_ERR
 */

// set nonzero by daemon_init()
int daemon_proc;

// initialize daemon_proc
void daemon_init(int num);

// Nonfatal error related to a system call.
// Print a message and return.
void err_ret(const char *fmt, ...);

// Fatal error related to a system call.
// Print a message and terminate.
void err_sys(const char *fmt, ...);

// Fatal error related to a system call.
// Print a message, dump core, and terminate.
void err_dump(const char *fmt, ...);

// Nonfatal error unrelated to a system call.
// Print a message and return.
void err_msg(const char *fmt, ...);

// Fatal error unrelated to a system call.
// Print a message and terminate.
void err_quit(const char *fmt, ...);

#define LOG_FUNC_BEGIN \
    printf("[%s][%d][%s] [begin]\n",__FILE__, __LINE__, __FUNCTION__);
#define LOG_FUNC_END \
    printf("[%s][%d][%s] [end]\n",__FILE__, __LINE__, __FUNCTION__);

#endif // _LOG_H
