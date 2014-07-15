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
#include <string.h>


static void
sig_handler(int sig)
{
    /* UNSAFE (see section 21.1.2) */
    printf("unset handler of %s(%d)\n", strsignal(sig), sig);
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int j;
    struct sigaction act;
    sigset_t empty_mask;

    sigemptyset(&empty_mask);
    act.sa_handler = sig_handler;
    act.sa_mask = empty_mask;
    act.sa_flags = SA_RESETHAND;
    for (j = 1; j < NSIG; ++j) {
        (void) sigaction(j, &act, NULL);
    }

    while (1) {
        pause();
    }

    exit(EXIT_SUCCESS);
}
