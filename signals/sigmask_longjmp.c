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


/* sigmask_longjmp.c

   Demonstrate the different effects of longjmp() and siglongjmp()
   on the process signal mask.

   By default, this program uses setjmp() + longjmp(). Compile with
   -DUSE_SIGSETJMP to use sigsetjmp() + siglongjmp().
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>

#include "signal_functions.h"


/* set to 1 once 'env' buffer has been initialized by [sig]setjmp() */
static volatile sig_atomic_t can_jump = 0;


#ifdef USE_SIGSETJMP
static sigjmp_buf senv;
#else
static jmp_buf env;
#endif


static void
handler(int sig)
{
    /* UNSAFE: This handler uses non-async-signal-safe functions
     * (printf(), strsignal(), print_sigmask(); see section 21.1.2) */
    printf("Received signal %d (%s), signal mask is:\n",
            sig, strsignal(sig));
    print_sigmask(stdout, NULL);
    if (!can_jump) {
        printf("'env' buffer not yet set, doing a simple return\n");
        return;
    }
#ifdef USE_SIGSETJMP
    siglongjmp(senv, 1);
#else
    longjmp(env, 1);
#endif
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct sigaction sa;
    print_sigmask(stdout, "Signal mask at startup:\n");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

#ifdef USE_SIGSETJMP
    printf("Calling sigsetjmp()\n");
    if (sigsetjmp(senv, 1) == 0)
#else
    printf("Calling setjmp()\n");
    if (setjmp(env) == 0)
#endif
    {
        can_jump = 1;
    } else {
        print_sigmask(stdout,
                "After jump from handler, signal  mask is:\n");
    }

    while (1) {
        pause();
    }

    exit(EXIT_SUCCESS);
}
