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


/* t_sigaltstack.c

   Demonstrate the use of sigaltstack() to handle a signal on an alternate
   signal stack.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>


static void
sigsegv_handler(int sig)
{
    int x;

    /* UNSAFE: This handler uses non-async-signal-safe functions
     * (printf(), strsignal(), fflush(): see section 21.1.2)
     */
    printf("Caught signal %d (%s)\n", sig, strsignal(sig));
    printf("Top of handler stack near     %10p\n", (void*) &x);
    fflush(NULL);

    _exit(EXIT_FAILURE);        /* Cannot return after SIGSEGV */
}


static void
overflow_stack(int call_num)
{
    /* A recursive function that overflows the stack */

    char a[100000]; /* Make this stack frame large */
    printf("Call %4d - top of stack near %10p\n", call_num, &a[0]);
    overflow_stack(call_num+1);
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    stack_t sigstack;
    struct sigaction sa;
    int j;

    printf("Top of standard stack is near %10p\n", (void*) &j);

    /* Allocate alternate stack and inform kernel of its existence */
    sigstack.ss_sp = malloc(SIGSTKSZ);
    if (sigstack.ss_sp == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    sigstack.ss_size = SIGSTKSZ;
    sigstack.ss_flags = 0;
    if (sigaltstack(&sigstack, NULL) == -1) {
        perror("signaltstack");
        exit(EXIT_FAILURE);
    }
    printf("Alternate stack is at         %10p-%p\n",
            sigstack.ss_sp, (char*) sbrk(0) - 1);

    sa.sa_handler = sigsegv_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_ONSTACK;
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    overflow_stack(1);
}
