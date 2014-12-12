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


/* multi_SIGCHLD.c

   Demonstrate the use of a handler for the SIGCHLD signal, and that multiple
   SIGCHLD signals are not queued while the signal is blocked during the
   execution of the handler.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#include "print_wait_status.h"


static volatile int num_live_children = 0;
    /* Number of children started but not yet waited on */


static char * now(char const *format);


static void
sigchld_handler(int sig __attribute__((unused)))
{
    int status, saved_errno;
    pid_t cpid;

    /*
     * UNSAFE: This handler uses non-async-signal-safe functions
     * (printf(), print_wait_status(), now(); see section 21.1.2)
     */

    saved_errno = errno;
        /* In case we modify 'errno' */
    printf("%s handler: Caught SIGCHLD\n", now("%T"));

    while ((cpid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("%s handler: Reaped child %ld - ",
                now("%T"), (long) cpid);
        print_wait_status(NULL, status);
        num_live_children--;
    }
    if (cpid == -1 && errno != ECHILD) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    sleep(5); /* Artificially lengthen execution of handler */
    printf("%s handler: returning\n", now("%T"));

    errno = saved_errno;
}


int
main(int argc, char *argv[])
{
    int j, sigcnt, slptime;
    sigset_t block_mask, empty_mask;
    struct sigaction sa;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s child-sleep-time...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Disable buffering of stdout */
    setbuf(stdout, NULL);

    sigcnt = 0;
    num_live_children = argc - 1;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigchld_handler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Block SIGCHLD to prevent its delivery if a child terminates
     * before the parent commences the sigsuspend() loop below */
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGCHLD);
    if (sigprocmask(SIG_SETMASK, &block_mask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    for (j = 1; j < argc; ++j) {
        switch (fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);

        case 0:
            /* Child - sleeps and then exits */
            slptime = atoi(argv[j]);
            if (slptime < 0) {
                fprintf(stderr, "child-sleep-time[%d] %s >= 0",
                        j-1, argv[j]);
                exit(EXIT_FAILURE);
            }
            sleep(slptime);
            printf("%s Child %d (PID=%ld) exiting\n",
                    now("%T"), j, (long) getpid());
            _exit(EXIT_SUCCESS);

        default:
                /* Parent - loops to create next child */
            break;
        }
    }

    /* Parent comes here: wait for SIGCHLD until all children are dead */
    sigemptyset(&empty_mask);
    while (num_live_children > 0) {
        if (sigsuspend(&empty_mask) == -1 && errno != EINTR) {
            perror("sigsuspend");
            exit(EXIT_FAILURE);
        }
        sigcnt++;
    }

    printf("%s All %d children have terminated; "
            "SIGCHLD was caught %d times\n",
            now("%T"), argc - 1, sigcnt);

    exit(EXIT_SUCCESS);
}


static char *
now(char const *format)
{
    static char buf[BUFSIZ];
    size_t s;
    time_t t;
    struct tm *st;

    t = time(NULL);
    st = localtime(&t);
    if (st == NULL) {
        return NULL;
    }
    s = strftime(buf, BUFSIZ, format == NULL ? "%c" : format, st);
    return s == 0 ? NULL : buf;
}
