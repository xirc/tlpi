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


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>


/* Handler for SIGHUP */
static void
handler(int sig)
{
    printf("PID %ld: caught signal %2d (%s)\n",
            (long) getpid(), sig, strsignal(sig));
        /* UNSAFE (see Section 21.1.2) */
}


int
main(int argc, char *argv[])
{
    pid_t ppid, cpid;
    int j;
    struct sigaction sa;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s {d|s}... [ > sig.log 2>&1 \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Make stdout unbuffered */
    setbuf(stdout, NULL);

    ppid = getpid();
    printf("PID of parent process is:      %ld\n", (long) ppid);
    printf("Foregroud process group ID is: %ld\n",
            (long) tcgetpgrp(STDIN_FILENO));

    /* Create child processes */
    for (j = 1; j < argc; ++j) {
        cpid = fork();
        if (cpid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (cpid == 0) {            /* If child ... */
            if (argv[j][0] == 'd'){ /* 'd' --> to different pgrp */
                if (setpgid(0,0) == -1) {
                    perror("setpgid");
                    exit(EXIT_FAILURE);
                }
            }
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sa.sa_handler = handler;
            if (sigaction(SIGHUP, &sa, NULL) == -1) {
                perror("sigaction");
                exit(EXIT_FAILURE);
            }
            break;      /* Child exits loop */
        }
    }

    /* Parent also handle SIGHUP */
    if (cpid > 0) { /* Parent */
        sigemptyset(&sa.sa_mask);
        sa.sa_handler = 0;
        sa.sa_handler = handler;
        if (sigaction(SIGHUP, &sa, NULL) == -1) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
    }

    /* All processes fall through to here */

    alarm(60);      /* Ensure each process eventually terminates */

    printf("PID=%ld PGID=%ld\n", (long) getpid(), (long) getpgrp());
    while (1) {
        pause();
    }

    exit(EXIT_SUCCESS);
}
