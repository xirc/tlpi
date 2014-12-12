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
#include <signal.h>
#include <string.h>
#include <libgen.h>
#include <sys/wait.h>

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
        exit(EXIT_SUCCESS);

    default:
        /* Wait child exits */
        if (waitid(P_ALL, 0, NULL, WEXITED | WNOWAIT) == -1) {
            perror("waitid");
            exit(EXIT_FAILURE);
        }
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

        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
        printf("After wait to zombie (PID=%ld):\n", (long) cpid);
        system(cmd); /* View no zombie child */

        exit(EXIT_SUCCESS);
    }
}
