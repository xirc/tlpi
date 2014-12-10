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
#include <sys/wait.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int status;

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        _exit(-1);
    default:
        if (wait(&status) == -1 && errno != ECHILD) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
        printf("%d\n", WEXITSTATUS(status));
    }

    exit(EXIT_SUCCESS);
}
