/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2014.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/
/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
* See above.                                                              *
\*************************************************************************/


/* fork_sig_sync.c

   Demonstrate how signals can be used to synchronize the actions
   of a parent and child process.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>


static char *
now(char const *format);


/* Synchronization signal */
#define SYNC_SIG SIGUSR1


/* Signal handler - does nothing but return */
static void
handler(int sig __attribute__((unused)))
{
    /* do nothing */
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    pid_t cpid;
    sigset_t block_mask, orig_mask, empty_mask;
    struct sigaction sa;

    setbuf(stdout, NULL);
        /* Disable buffering of stdout */

    sigemptyset(&block_mask);
    sigaddset(&block_mask, SYNC_SIG);
    if (sigprocmask(SIG_BLOCK, &block_mask, &orig_mask) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(SYNC_SIG, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    switch (cpid = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:
        /* Child does some required action here... */
        printf("[%s %ld] Child started - doing some work\n",
                now("%T"), (long) getpid());
        sleep(2);
            /* Simulate time spent doing some work */
        /* And then signals parent that is's done */
        printf("[%s %ld] Child about to signal parent\n",
                now("%T"), (long) getpid());
        if (kill(getppid(), SYNC_SIG) == -1) {
            perror("kill");
            exit(EXIT_FAILURE);
        }
        /* Now child can do other things... */
        _exit(EXIT_FAILURE);

    default:
        /* Parent may do some work here,
         * and then waits for child to complete the required action */
        printf("[%s %ld] Parent about to wait for signal\n",
                now("%T"), (long) getpid());
        sigemptyset(&empty_mask);
        if (sigsuspend(&empty_mask) == -1 && errno != EINTR) {
            perror("sigsuspend");
            exit(EXIT_FAILURE);
        }
        printf("[%s %ld] Parent got signal\n",
                now("%T"), (long) getpid());

        /* If required, return signal mask to its original state */
        if (sigprocmask(SIG_SETMASK, &orig_mask, NULL) == -1) {
            perror("sigprocmask");
            exit(EXIT_FAILURE);
        }
        /* Parent carries on to do other things... */

        exit(EXIT_SUCCESS);
    }
}


#define BUF_SIZE 1024
static char *
now(char const *format)
{
    static char buf[BUF_SIZE];
    time_t t;
    size_t s;
    struct tm *tm;

    t = time(NULL);
    tm = localtime(&t);
    if (tm == NULL) {
        return NULL;
    }
    s = strftime(buf, BUF_SIZE, (format != NULL) ? format : "%c", tm);
    return (s == 0) ? NULL : buf;
}
