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


/* pipe_ls_wc.c

   Demonstrate the use of a pipe to connect two filters. We use fork()
   to create two children. The first one execs ls(1), which writes to
   the pipe, the second execs wc(1) to read from the pipe.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int pfd[2];

    /* Pipe file descriptors */
    /* Create pipe */
    if (pipe(pfd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:
        /* First child: exec 'ls' to write to pipe */
        /* Read end is unused */
        if (close(pfd[0]) == -1) {
            perror("close 1");
            exit(EXIT_FAILURE);
        }

        /* Duplicate stdout on write end of pipe;
         * close duplicated descriptor */
        if (pfd[1] != STDOUT_FILENO) { /* Defensive check */
            if (dup2(pfd[1], STDOUT_FILENO) == -1) {
                perror("dup2 1");
                exit(EXIT_FAILURE);
            }
            if (close(pfd[1]) == -1) {
                perror("close 2");
                exit(EXIT_FAILURE);
            }
        }

        /* Write to pipe */
        execlp("ls", "ls", (char *) NULL);
        perror("execlp ls");
        exit(EXIT_FAILURE);

    default:
        /* Parent falls through to create next child */
        break;
    }

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:
        /* Second child: exec 'wc' to read from pipe */
        /* Write end is unused */
        if (close(pfd[1]) == -1) {
            perror("close 3");
            exit(EXIT_FAILURE);
        }

        /* Duplicate stdin on read end of pipe;
         * close duplicated descriptor */
        if (pfd[0] != STDIN_FILENO) { /* Defensive check */
            if (dup2(pfd[0], STDIN_FILENO) == -1) {
                perror("dup2 2");
                exit(EXIT_FAILURE);
            }
            if (close(pfd[0]) == -1) {
                perror("close 4");
                exit(EXIT_FAILURE);
            }
        }

        /* Reads from pipe */
        execlp("wc", "wc", "-l", (char *) NULL);
        perror("execlp wc");
        exit(EXIT_FAILURE);

    default:
        /* Parent falls through */
        break;
    }

    /* Parent closes unused file descriptors for pipe,
     * and waits for children */
    if (close(pfd[0]) == -1) {
        perror("close 5");
        exit(EXIT_FAILURE);
    }
    if (close(pfd[1]) == -1) {
        perror("close 6");
        exit(EXIT_FAILURE);
    }
    if (wait(NULL) == -1) {
        perror("wait 1");
        exit(EXIT_FAILURE);
    }
    if (wait(NULL) == -1) {
        perror("wait 2");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
