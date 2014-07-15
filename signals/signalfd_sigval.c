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


/* signalfd_sigval.c

   Usage: signalfd_sigval sig-num...

   Demonstrate the use of signalfd() to receive signals via a file descriptor.

   This program is Linux-specific. The signalfd API is supported since kernel
   2.6.22.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/signalfd.h>
#include <signal.h>
#include <string.h>


int
main(int argc, char *argv[])
{
    sigset_t mask;
    int sfd, j;
    struct signalfd_siginfo fdsi;
    ssize_t s;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s sig-num...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("%s: PID = %ld\n", argv[0], (long) getpid());

    sigemptyset(&mask);
    for (j = 1; j < argc; j++) {
        sigaddset(&mask, atoi(argv[j]));
    }

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    sfd = signalfd(-1, &mask, 0);
    if (sfd == -1) {
        perror("signalfd");
        exit(EXIT_FAILURE);
    }

    while (1) {
        s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
        if (s != sizeof(struct signalfd_siginfo)) {
            fprintf(stderr, "read error\n");
            exit(EXIT_FAILURE);
        }

        printf("%s: got signal %d", argv[0], fdsi.ssi_signo);
        if (fdsi.ssi_code == SI_QUEUE) {
            printf("; ssi_pid = %d; ", fdsi.ssi_pid);
            printf("ssi_int = %d", fdsi.ssi_int);
        }
        printf("\n");
    }
}
