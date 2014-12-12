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
#include <sys/wait.h>
#include <signal.h>

#include "print_wait_status.h"


int
main(int argc, char *argv[])
{
    pid_t cpid;
    siginfo_t si;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [exit-status]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
    case -1:
        perror("fork()");
        exit(EXIT_FAILURE);

    case 0:
        /* Child: either exits immediately with given
         * status or loops waiting for signals */
        printf("Child started with PID = %ld\n", (long) getpid());
        if (argc > 1) {
            exit(atoi(argv[1]));
        } else {
            while (1) {
                pause();
            }
        }
        exit(EXIT_FAILURE); /* Not reached, but good practice */

    default:
        /* Parent: repeatedly wait on child until it
         * either exits or is terminated by a signal */
        while (1) {
            cpid = waitid(P_ALL, 0, &si, WEXITED | WSTOPPED | WCONTINUED);
            if (cpid == -1) {
                perror("waitid");
                exit(EXIT_FAILURE);
            }

            /* Print status in hex, and as separate decimal bytes */
            printf("waitid() returned:\n"
                    "\tcode=%d, pid=%ld, signo=%d, status=%d, uid=%d\n",
                    si.si_code, (long) si.si_pid,
                    si.si_signo, si.si_status, si.si_uid);
            print_wait_status2(NULL, &si);
            if (si.si_code == CLD_EXITED ||
                si.si_code == CLD_KILLED)
            {
                exit(EXIT_FAILURE);
            }
        }
    }
}
