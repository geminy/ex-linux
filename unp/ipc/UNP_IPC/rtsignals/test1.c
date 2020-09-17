#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

typedef void sigfunc_rt(int, siginfo_t*, void*);

static void sig_rt(int, siginfo_t*, void*);
sigfunc_rt* signal_rt(int, sigfunc_rt*);

int main(int argc, char **argv)
{
    int i, j;
    pid_t pid;
    sigset_t newset;
    union sigval val;
    printf("SIGRTMIN = %d, SIGRTMAX = %d\n", (int)SIGRTMIN, (int)SIGRTMAX);

    if (0 == (pid = fork())) {
        // child: block three realtime signals
        if (0 != sigemptyset(&newset)) {
            printf("sigemptyset error\n");
        }
        if (0 != sigaddset(&newset, SIGRTMAX)) {
            printf("sigaddset error: SIGRTMAX\n");
        }
        if (0 != sigaddset(&newset, SIGRTMAX - 1)) {
            printf("sigaddset error: SIGRTMAX - 1\n");
        }
        if (0 != sigaddset(&newset, SIGRTMAX - 2)) {
            printf("sigaddset error: SIGRTMAX - 2\n");
        }
        if (0 != sigprocmask(SIG_BLOCK, &newset, NULL)) {
            printf("sigprocmask error: %s\n", strerror(errno));
        }
        // establish signal handler with SA_SIGINFO set
        signal_rt(SIGRTMAX, sig_rt);
        signal_rt(SIGRTMAX - 1, sig_rt);
        signal_rt(SIGRTMAX - 2, sig_rt);
        sleep(6); // let parent send all the signals
        if (0 != sigprocmask(SIG_UNBLOCK, &newset, NULL)) { // unblock
            printf("sigprocmask error: %s\n", strerror(errno));
        }
        sleep(3); // let all queued signals be delivered
        exit(0);
    }
    // parent sends nine signals to child
    sleep(3); // let child block all signals
    for (i = SIGRTMAX; i >= SIGRTMAX - 2; i--) {
        for (j = 0; j <= 2; j++) {
            val.sival_int = j;
            if (0 != sigqueue(pid, i, val)) {
                printf("sigqueue error: %s\n", strerror(errno));
            }
            printf("sent signal %d, val = %d\n", i, j);
        }
    }
    exit(0);
}

static void sig_rt(int signo, siginfo_t *info, void *context)
{
    printf("received signal %d, code = %d, ival = %d\n", signo, info->si_code, info->si_value.sival_int);
}

sigfunc_rt* signal_rt(int signo, sigfunc_rt *func)
{
    struct sigaction act, oact;
    act.sa_sigaction = func; // must store function addr here
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT; // SunOS 4.x
#endif
    }
    else {
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART; // SVR4, 44BSD
#endif
    }
    if (sigaction(signo, &act, &oact) < 0) {
        return (sigfunc_rt*)SIG_ERR;
    }
    return oact.sa_sigaction;
}
