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
#include <signal.h>
#include <sys/wait.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int retc;

    /* for core dump */
    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:
        /* Child send SIGABRT signal to itself. */
        if (kill(getpid(), SIGABRT) == -1) {
            perror("kill");
            exit(EXIT_FAILURE);
        }
        _exit(EXIT_FAILURE);

    default:
        /* Parent wait child exits */
        if (wait(&retc) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
        printf("Child exits with return code (%d)\n", retc);
        if (retc >= 128 &&  retc - 128 == SIGABRT) {
            printf("Child may generate core dump\n");
            printf("If you find core dump,"
                " you execute 'ulimit -c unlimited' and execute me again!\n");
        }
        printf("Parent continue to execute\n");
        while (1) {
            pause();
        }
    }
}
