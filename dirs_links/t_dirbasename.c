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


/* t_dirbasename.c

   Demonstrate the use of dirname() and basename() to break a pathname
   into directory and filename components.

   Usage: t_dirbasename path...

   The program calls dirname() and basename() for each of the pathnames
   supplied on the command-line.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <libgen.h>


int
main(int argc, char *argv[])
{
    char *t1, *t2;
    int i;

    for (i = 1; i < argc; ++i) {
        t1 = strdup(argv[i]);
        if (t1 == NULL) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
        t2 = strdup(argv[i]);
        if (t2 == NULL) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }

        printf("%s ==> %s + %s\n", argv[i], dirname(t1), basename(t2));

        free(t1);
        free(t2);
    }

    exit(EXIT_SUCCESS);
}
