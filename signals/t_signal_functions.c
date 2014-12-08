/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "signal_functions.h"


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    sigset_t mask;

    print_sigmask(stdout, "MASK    ");
    print_pending_sigs(stdout, "PENDINGS");

    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);

    printf("SET MASK\n");
    print_sigset(stdout, "  ", &mask);
    sigprocmask(SIG_SETMASK,&mask, NULL);

    print_sigmask(stdout, "MASK\n");
    print_pending_sigs(stdout, "PENDINGS");

    printf("kill SIGUSR1\n");
    if (kill(0, SIGUSR1) == -1) {
        perror("kill");
        exit(EXIT_FAILURE);
    }

    print_sigmask(stdout, "MASK\n");
    print_pending_sigs(stdout, "PENDINGS");

    exit(EXIT_SUCCESS);
}
