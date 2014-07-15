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


/* intquit.c

   Catch the SIGINT and SIGQUIT signals, which are normally generated
   by the control-C (^C) and control-\ (^\) keys respectively.

   Note that although we use signal() to establish signal handlers in this
   program, the use of sigaction() is always preferable for this task.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>


static void
sig_handler(int sig)
{
    static int count = 0;

    /* UNSAFE: This handler uses non-async-signal-safe functions
     * (printf(), exit(); see section 21.1.2) */

    if (sig == SIGINT) {
        ++count;
        printf("Caught SIGINT (%d)\n", count);
        return; /* resume execution at point of interruption */
    }

    /* Must be SIGQUIT - print a message and terminate the process */
    printf("Caught SIGQUIT - that's all folks!\n");
    exit(EXIT_SUCCESS);
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    /* Establish same handler for SIGINT and SIGQUIT */
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGQUIT, sig_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    while (1) {
        /* Loop forever, waiting for signals
         * Block until a signal is caught */
        pause();
    }
}
