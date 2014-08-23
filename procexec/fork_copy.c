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
#include <sys/wait.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    pid_t cpid;
    int status;

    printf("PARENT: PID=%d PPID=%d\n", getpid(), getppid());
    fflush(stdout);

    cpid = fork();
    switch (cpid) {
    case -1:
        perror("fork1");
        exit(EXIT_FAILURE);

    case 0: /* Child */
        switch (fork()) {
        case -1:
            perror("fork2");
            exit(EXIT_FAILURE);

        case 0: /* Grandchild */
            /* --- Do real work here --- */
            sleep(1);
            /* Grandchild may be orphan */
            printf("GRANDCHILD: PID=%d PPID=%d\n", getpid(), getppid());
            exit(EXIT_SUCCESS);

        default:
            /* Make grandchild an orphan */
            printf("CHILD: PID=%d PPID=%d\n", getpid(), getppid());
            exit(EXIT_SUCCESS);
        }

    default:
        break;
    }

    /* Parent falls through to here */
    if (waitpid(cpid, &status, 0) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    /* Parent carries on to do other things. */

    exit(EXIT_FAILURE);
}
