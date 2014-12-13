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


int
main(int argc, char *argv[])
{
    pid_t pid;
    int nchilds;
    int j;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [num-childs]\n", argv[0]);
        fprintf(stderr, "e.g.) %s 5 | grep 'some'\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    nchilds = atoi(argv[1]);
    if (nchilds <= 0) {
        fprintf(stderr, "nchilds (%s) > 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Call fork() to create a number of child processes,
     * each of which remains in same process group as the parent */
    for (j = 0; j < nchilds; ++j) {
        pid = fork();
        switch(pid) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            pause();
            _exit(EXIT_SUCCESS);
        default:
            /* do nothing */
            break;
        }
    }

    /* Parent falls through to here after creating all children */

    /* Some thing later ... */
    sleep(3);

    /* Parent makes itself immune to SIGUSR1 */
    signal(SIGUSR1, SIG_IGN);

    /* Send signal to children created earlier */
    killpg(getpgrp(), SIGUSR1);

    exit(EXIT_SUCCESS);
}
