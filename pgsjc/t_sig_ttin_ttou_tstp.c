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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>


static void
handler(int sig)
{
    /* UNSAFE */
    printf("PROCESS(%ld) received signal (%d, %s)\n",
            (long) getpid(), sig, strsignal(sig));
}


int
main(int argc, char *argv[])
{
    int use_dfl;
    pid_t pid;
    struct sigaction sa;

    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [use-dfl-handler]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    use_dfl = (argc > 1) ? 1 : 0;

    pid = fork();
    if (pid != 0) {
        /* Parent make the child orphan */
        exit(EXIT_SUCCESS);
    }

    if (use_dfl == 0) {
        /* Child set handler for SIGTTIN, SIGTTOU, and SIGTSTP */
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = handler;

        if (sigaction(SIGTTIN, &sa, NULL) == -1) {
            perror("sigaction");
        }
        if (sigaction(SIGTTOU, &sa, NULL) == -1) {
            perror("sigaction");
        }
        if (sigaction(SIGTSTP, &sa, NULL) == -1) {
            perror("sigaction");
        }
    }

    printf("PID=%ld is zombie process. You should kill %ld at end.\n",
            (long) getpid(), (long) getpid());
    while (1) {
        pause();
    }

    exit(EXIT_SUCCESS);
}
