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


/* handling_SIGTSTP.c

   Demonstrate the correct way to catch SIGTSTP and raise it again (so that a
   parent process that is monitoring this program can see that it was stopped
   by SIGTSTP).
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>


/* Handler for SIGTSTP */
static void
tstp_handler(int sig __attribute__((unused)))
{
    sigset_t tstp_mask, prev_mask;
    int saved_errno;
    struct sigaction sa;

    saved_errno = errno;    /* In case we change 'errno' here */

    printf("Caught SIGTSTP\n"); /* UNSAFE (see Section 21.1.2) */

    /* Set handling to default */
    if (signal(SIGTSTP, SIG_DFL) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    /* Generate a further SIGTSTP */
    raise(SIGTSTP);

    /* Unblock SIGTSTP;
     * the pending SIGTSTP immediately suspends the program */
    sigemptyset(&tstp_mask);
    sigaddset(&tstp_mask, SIGTSTP);
    if (sigprocmask(SIG_UNBLOCK, &tstp_mask, &prev_mask) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    /* Execution resumes here after SIGCONT */

    /* Reblock SIGTSTP */
    if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    /* Reestablish handler */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = tstp_handler;
    if (sigaction(SIGTSTP, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("Exiting SIGTSTP handler\n");
    errno = saved_errno;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct sigaction sa;

    /* Only establish handler for SIGTSTP
     * if it is not being ignored */
    if (sigaction(SIGTSTP, NULL, &sa) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if (sa.sa_handler != SIG_IGN) {
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        sa.sa_handler = tstp_handler;
        if (sigaction(SIGTSTP, &sa, NULL) == -1) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
    }

    /* Wait for signals */
    while (1) {
        pause();
        printf("Main\n");
    }

    /* cannot reach here */
    exit(EXIT_SUCCESS);
}
