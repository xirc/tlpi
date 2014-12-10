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


/* t_fork.c

   Demonstrate the use of fork(), showing that parent and child
   get separate copies of stack and data segments.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static int idata = 111;


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int istack = 222;
        /* Allocated in stack segment */
    pid_t cpid;

    switch (cpid = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        idata *= 3;
        istack *= 3;
        break;
    default:
        sleep(3);
            /* Give child a chance to execute. */
        break;
    }

    /* Both parent and child come here */
    printf("PID=%ld %s idata=%d istack=%d\n",
            (long) getpid(),
            (cpid == 0) ? "(child)" : "(parent)", idata, istack);

    exit(EXIT_SUCCESS);
}
