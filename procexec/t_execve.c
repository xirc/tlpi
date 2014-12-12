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


/* t_execve.c

   Demonstrate the use of execve() to execute a program.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int
main(int argc,
     char *argv[])
{
    char *arg_vec[10]; /* larger than required */
    char *env_vec[] = { "GREET=salut", "BYE=adieu", NULL };

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s pathname\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    arg_vec[0] = strrchr(argv[1], '/');
    if (arg_vec[0] != NULL) {
        arg_vec[0]++;
    } else {
        arg_vec[0] = argv[1];
    }
    arg_vec[1] = "hello world";
    arg_vec[2] = "goodbye";
    arg_vec[3] = NULL;  /* List must be NULL-terminated */

    execve(argv[1], arg_vec, env_vec);

    /* If we get here, something went wrong */
    perror("execve");
    exit(EXIT_FAILURE);
}
