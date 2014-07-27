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


/* t_system.c

   Demonstrate the use of system(3) to execute a shell command.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#include "print_wait_status.h"


#define MAX_CMD_LEN 256


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    /* Command to be executed by system() */
    char str[MAX_CMD_LEN];
    /* Status return from system() */
    int status;

    while (1) {
        printf("Command: ");
        fflush(stdout);
        if (fgets(str, MAX_CMD_LEN, stdin) == NULL) {
            break; /* end-of-file */
        }

        status = system(str);
        printf("system() returned: status=0x%04x (%d,%d)\n",
                (unsigned int) status, status >> 8, status & 0xFF);

        if (status == -1) {
            perror("system");
            exit(EXIT_FAILURE);
        } else {
            if (WIFEXITED(status) && WEXITSTATUS(status) == 127) {
                printf("(Probably) could not invoke shell\n");
            } else {
                print_wait_status(NULL, status);
            }
        }
    }

    exit(EXIT_SUCCESS);
}
