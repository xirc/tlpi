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


/* simple_pipe.c

   Simple demonstration of the use of a pipe to communicate
   between a parent and a child process.

   Usage: simple_pipe "string"

   The program creates a pipe, and then calls fork() to create a child process.
   After the fork(), the parent writes the string given on the command line
   to the pipe, and the child uses a loop to read data from the pipe and
   print it on standard output.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>


#define BUF_SIZE 10


int
main(int argc, char *argv[])
{
    int pfd[2];     /* Pipe file descriptor */
    char buf[BUF_SIZE];
    ssize_t num_read;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s string\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (pipe(pfd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        /* Child - reads from pipe */
        /* Write end is unused */
        if (close(pfd[1]) == -1) {
            perror("close -child");
            exit(EXIT_FAILURE);
        }

        /* Read data from pipe, echo on stdout */
        while (1) {
            num_read = read(pfd[0], buf, BUF_SIZE);
            if (num_read == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            if (num_read == 0) {
                break;
            }
            if (write(STDOUT_FILENO, buf, num_read) != num_read) {
                fprintf(stderr, "child - partial/failed write\n");
                exit(EXIT_FAILURE);
            }
        }

        write(STDOUT_FILENO, "\n", 1);
        if (close(pfd[0]) == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }

        _exit(EXIT_SUCCESS);
    default:
        /* Parent - writes to pipe */
        if (close(pfd[0]) == -1) {
            perror("close -parent");
            exit(EXIT_FAILURE);
        }

        if ((size_t) write(pfd[1], argv[1], strlen(argv[1])) !=
                strlen(argv[1]))
        {
            fprintf(stderr, "parent - partial/failed write\n");
            exit(EXIT_FAILURE);
        }

        /* Child will see EOF */
        if (close(pfd[1]) == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }

        /* Wait for child to finish */
        wait(NULL);
        exit(EXIT_SUCCESS);
    }

    /* Cannot reach here */
}
