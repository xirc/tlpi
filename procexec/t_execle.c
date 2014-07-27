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


/* t_execle.c

   Demonstrate the use of execle() to execute a program.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int
main(int argc, char *argv[])
{
    char *envvec[] = { "GREET=salut", "BYE=adieu", NULL };
    char *filename;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s pathname\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Get basename from argv[1] */
    filename = strrchr(argv[1], '/');
    if (filename != NULL) {
        filename++;
    } else {
        filename = argv[1];
    }

    execle(argv[1], filename, "hello world", (char *) NULL, envvec);

    /* If we get here, something went wrong */
    perror("execle");
    exit(EXIT_FAILURE);
}
