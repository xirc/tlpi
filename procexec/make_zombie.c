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


/* make_zombie.c

   Demonstrate how a child process becomes a zombie in the interval between
   the time it exits, and the time its parent performs a wait (or exits, at
   which time it is adopted by init(8), which does a wait, thus releasing
   the zombie).
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <libgen.h>

#define CMD_SIZE 256


int
main(int argc __attribute__((unused)),
     char *argv[])
{
    char cmd[CMD_SIZE];
    pid_t cpid;

    /* Disable buffering of stdout */
    setbuf(stdout, NULL);

    printf("Parent PID=%ld\n", (long) getpid());

    switch (cpid = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:
        /* Child: immediately exits to become zombie */
        printf("Child (PID=%ld) exiting\n", (long) getpid());
        _exit(EXIT_SUCCESS);

    default:
        sleep(3); /* Give child a chance to start and exit */
        snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
        system(cmd); /* View zombie child */

        /* Now send the "sure kill" signal to the zombie */
        if (kill(cpid, SIGKILL) == -1) {
            perror("kill");
            exit(EXIT_FAILURE);
        }
        sleep(3); /* Give child a chance to react to signal */
        printf("After sending SIGKILL to zombie (PID=%ld):\n",
                (long) cpid);
        system(cmd); /* View zombie child again */

        exit(EXIT_SUCCESS);
    }
}
