/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _BSD_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int istack = 222;

    switch (vfork()) {
    case -1:
        perror("vfork");
        exit(EXIT_FAILURE);
    case 0:
        /* Child executes first, in parent's memory space
         * Even if we sleep for a while, parent still is not scheduled. */
        sleep(3);
        write(STDOUT_FILENO, "Child executing\n", 16);
        if (close(STDOUT_FILENO) == -1) {
            perror("close STDOUT");
            exit(EXIT_FAILURE);
        }
        istack *= 3;
        _exit(EXIT_SUCCESS);
    default:
        /* Parent is blocked until child exits. */
        write(STDOUT_FILENO, "Parent executing\n", 17);
        printf("istack=%d\n", istack);
        exit(EXIT_SUCCESS);
    }
}
