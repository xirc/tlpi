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


/* t_execl.c

   Demonstrate the use of execl() to execute printenv(1).
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    printf("Initial value of USER: %s\n", getenv("USER"));
    printf("Initial value of SHELL: %s\n", getenv("SHELL"));

    if (putenv("USER=britta") != 0) {
        perror("putenv");
        exit(EXIT_FAILURE);
    }

    execl("/usr/bin/printenv", "printenv", "USER", "SHELL", (char *) NULL);

    /* If we get here, something went wrong */
    perror("execl");
    exit(EXIT_FAILURE);
}
