/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>


void m_abort()
{
    struct sigaction sa;
    sigset_t mask;

    /* Flush and close stdout */
    fflush(stdout);
    fclose(stdout);

    /* Send a signal 'SIGABRT' to myself */
    raise(SIGABRT);

    /* Signal was ignored,
     * - unset signal mask of SIGABRT
     * - set default signal handler
     * and then resend a signal to myself */
    sigemptyset(&mask);
    sigaddset(&mask, SIGABRT);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    sa.sa_handler = SIG_DFL;
    sigaction(SIGABRT, &sa, NULL);
    raise(SIGABRT);
}


int main()
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGABRT);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }
    m_abort();
}
