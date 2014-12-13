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


/* catch_SIGHUP.c

   Catch the SIGHUP signal and display a message.

   Usage: catch_SIGHUP [x] [ > logfile 2>&1 ]
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>


static void
handler(int sig __attribute__((unused)))
{
    /* do nothing */
}


int
main(int argc,
     char *argv[] __attribute__((unused)))
{
    pid_t cpid;
    struct sigaction sa;

    /* Make stdout unbuffering */
    setbuf(stdout, NULL);

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(SIGHUP, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0 && argc > 1) {
        if (setpgid(0, 0) == -1) { /* Move to new process group */
            perror("setpgid");
            exit(EXIT_FAILURE);
        }
    }

    /* Parent and child come here */

    printf("PID=%ld; PPID=%ld; PGID=%ld; SID=%ld\n",
            (long) getpid(), (long) getppid(),
            (long) getpgrp(), (long) getsid(0));

    /* An unhandled SIGALRM ensures this process
     * will be die if nothing else terminates it */
    alarm(60);

    while (1) {
        pause();
        printf("%ld: caught SIGHUP\n", (long) getpid());
    }

    exit(EXIT_SUCCESS);
}
