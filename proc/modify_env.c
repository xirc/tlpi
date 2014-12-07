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


/* modify_env.c

   Demonstrate modification of the process environment list.

   Usage: modify_env name=value...

   Note: some UNIX implementations do not provide clearenv(), setenv(),
   and unsetenv().
*/


#define _GNU_SOURCE     /* Get various declarations from <stdlib.h> */
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


extern char **environ;

int
main(int argc, char *argv[])
{
    int j;
    char **ep;

    clearenv();         /* Erase entire environment */

    /* Add any definitions specified on command line to environment */
    for (j = 1; j < argc; j++) {
        if (putenv(argv[j]) != 0) {
            fprintf(stderr, "putenv %s : %s\n", argv[j], strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    /* Add a definition for GREET if one does not already exist */
    if (setenv("GREET", "Hello world", 0) == -1) {
        perror("setenv");
        exit(EXIT_FAILURE);
    }

    /* Remove any existing definition of BYE */
    unsetenv("BYE");

    /* Display current environment */
    for (ep = environ; *ep != NULL; ep++) {
        puts(*ep);
    }

    exit(EXIT_SUCCESS);
}
