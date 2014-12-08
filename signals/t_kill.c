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


/* t_kill.c

   Send a signal using kill(2) and analyze the return status of the call.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>


int
main(int argc, char *argv[])
{
    int s, sig, pid;

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s pid sig-num\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid = atoi(argv[1]);
    sig = atoi(argv[2]);
    if (sig < 0) {
        fprintf(stderr, "sig-num %s > 0\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    s = kill(pid, sig);
    if (sig != 0) {
        if (s == -1) {
            perror("kill");
            exit(EXIT_FAILURE);
        }
    } else { /* Null signal: process existence check */
        if (s == 0) {
            printf("Process exists and we can send it a signal\n");
        } else {
            if (errno == EPERM) {
                printf("Process exists, but we don't have "
                       "permission to send it a signal\n");
            } else if (errno == ESRCH) {
                printf("Process does not exists\n");
            } else {
                perror("kill");
            }
        }
    }

    exit(EXIT_SUCCESS);
}
