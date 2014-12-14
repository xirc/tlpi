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


/* svmsg_rm.c

   Remove the System V message queues identified by the command-line arguments.
*/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int
main(int argc, char *argv[])
{
    int j;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [msqid...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (j = 1; j < argc; ++j) {
        if (msgctl(atoi(argv[j]), IPC_RMID, NULL) == -1) {
            fprintf(stderr, "msgctl %s: %s\n", argv[j], strerror(errno));
        }
    }

    exit(EXIT_SUCCESS);
}
