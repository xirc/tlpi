/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2014.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/
/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
* See above.                                                              *
\*************************************************************************/


/* signal_functions.c

   Various useful functions for working with signals.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "signal_functions.h"


/* NOTE: All of the following functions employ fprintf(), which
   is not async-signal-safe (see Section 21.1.2). As such, these
   functions are also not async-signal-safe (i.e., beware of
   indiscriminately calling them from signal handlers). */


/* Print list of signals within a signal set */
void
print_sigset(FILE *of, const char *prefix, const sigset_t *sigset)
{
    int sig, cnt;

    cnt = 0;
    for (sig = 1; sig < NSIG; sig++) {
        if (sigismember(sigset, sig)) {
            cnt++;
            fprintf(of, "%s%d (%s)\n", prefix, sig, strsignal(sig));
        }
    }

    if (cnt == 0) {
        fprintf(of, "%s<empty signal set>\n", prefix);
    }
}


/* Print mask of blocked signals for this process */
int
print_sigmask(FILE *of, const char *msg)
{
    sigset_t currMask;

    if (msg != NULL) {
        fprintf(of, "%s", msg);
    }

    if (sigprocmask(SIG_BLOCK, NULL, &currMask) == -1) {
        return -1;
    }

    print_sigset(of, "\t\t", &currMask);

    return 0;
}


/* Print signals currently pending for this process */
int
print_pending_sigs(FILE *of, const char *msg)
{
    sigset_t pendingSigs;

    if (msg != NULL) {
        fprintf(of, "%s", msg);
    }

    if (sigpending(&pendingSigs) == -1) {
        return -1;
    }

    print_sigset(of, "\t\t", &pendingSigs);

    return 0;
}
