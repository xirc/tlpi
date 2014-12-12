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


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    /* Disable buffering of stdout */
    setbuf(stdout, NULL);

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:
        sleep(3); /* Give parent a chance to exit */
        printf("Child: PPID is %ld\n", (long) getppid());
        exit(EXIT_SUCCESS);

    default:
        /* Parent dies immediately */
        exit(EXIT_SUCCESS);
    }
}
