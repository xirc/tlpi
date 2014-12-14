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


/* t_setpriority.c

   Demonstrate the use of setpriority(2) and getpriority(2) to change and
   retrieve a process's nice value.

   Usage: t_setpriority {p|g|u} id priority
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>


static void
usage(FILE *fp, char *prog)
{
        fprintf(fp,
                "usage: %s {p|g|u} who priority\n"
                "          set priority of:"
                "p=process; "
                "g=process group; "
                "u=process for user\n", prog);
}


int
main(int argc, char *argv[])
{
    int which, prio;
    id_t who;

    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }
    if (argc != 4 || strchr("pgu", argv[1][0]) == NULL) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Set nice value according to command-line arguments */
    which = (argv[1][0] == 'p') ? PRIO_PROCESS :
                (argv[1][0] == 'g') ? PRIO_PGRP : PRIO_USER;
    who = atol(argv[2]);
    prio = atoi(argv[3]);

    if (setpriority(which, who, prio) == -1) {
        perror("setpriority");
        exit(EXIT_FAILURE);
    }

    /* Retrieve nice value to check the change */

    errno = 0;  /* Because successful call may return -1 */
    prio = getpriority(which, who);
    if (prio == -1 && errno != 0) {
        perror("getpriority");
        exit(EXIT_FAILURE);
    }

    printf("Nice value = %d\n", prio);

    exit(EXIT_SUCCESS);
}
