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


/* closeonexec.c

   Demonstrate retrieving and updating of the file descriptor
   close-on-exec flag.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>


int
main(int argc, char *argv[])
{
    int flags;

    if (argc > 1) {
        flags = fcntl(STDOUT_FILENO, F_GETFD);
        if (flags == -1) {
            perror("fcntl - F_GETFD");
            exit(EXIT_FAILURE);
        }

        flags |= FD_CLOEXEC;

        if (fcntl(STDOUT_FILENO, F_SETFD, flags) == -1) {
            perror("fcntl - F_SETFD");
            exit(EXIT_FAILURE);
        }
    }

    execlp("ls", "ls", "-l", argv[0], (char *) NULL);

    /* If we get here, something went wrong */
    perror("execlp");
    exit(EXIT_FAILURE);
}
