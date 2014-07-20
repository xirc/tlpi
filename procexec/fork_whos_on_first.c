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


/* fork_whos_on_first.c

   Parent repeatedly creates a child, and then processes both race to be the
   first to print a message. (Each child terminates after printing its message.)
   The results of running this program give us an idea of which of the two
   processes--parent or child--is usually scheduled first after a fork().

   Whether the child or the parent is scheduled first after fork() has
   changed a number of times across different kernel versions.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>


int
main(int argc, char *argv[])
{
    int num_children, j;
    pid_t cpid;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [num-children]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    num_children = (argc > 1) ? atoi(argv[1]) : 1;
    if (num_children <= 0) {
        fprintf(stderr, "num-children %s > 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    setbuf(stdout, NULL);
        /* Make stdout unbuffered */

    for (j = 0; j < num_children; ++j) {
        switch (cpid = fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            printf("%d child\n", j);
            _exit(EXIT_SUCCESS);
        default:
            printf("%d parent\n", j);
            wait(NULL);
            break;
        }
    }

    exit(EXIT_SUCCESS);
}
