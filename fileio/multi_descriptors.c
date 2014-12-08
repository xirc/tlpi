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


/* multi_descriptors.c

   Show the interaction of multiple descriptors accessing the same
   file (some via the same shared open file table entry).
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define BUFSIZE 128


int
main(int argc, char *argv[])
{
    int fd1, fd2, fd3;

    /* check arguments */
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s pathname\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd1 = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd1 == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    fd2 = dup(fd1);
    if (fd2 == -1) {
        perror("dup");
        exit(EXIT_FAILURE);
    }
    fd3 = open(argv[1], O_RDWR);
    if (fd3 == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /* "Hello," */
    if (write(fd1, "Hello,", 6) != 6) {
        fprintf(stderr, "cannot write whole buffer\n");
        exit(EXIT_FAILURE);
    }

    /* "Hello, world" */
    if (write(fd2, " world", 6) != 6) {
        fprintf(stderr, "cannot write whole buffer\n");
        exit(EXIT_FAILURE);
    }

    /* "HELLO, world" */
    if (lseek(fd2, 0, SEEK_SET) == -1) {
        perror("seek");
        exit(EXIT_FAILURE);
    }
    if (write(fd1, "HELLO,", 6) != 6) {
        fprintf(stderr, "cannot write whole buffer\n");
        exit(EXIT_FAILURE);
    }

    /* "Gidday world" */
    if (write(fd3, "Gidday", 6) != 6) {
        fprintf(stderr, "cannot write whole buffer\n");
        exit(EXIT_FAILURE);
    }

    if (close(fd1) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
    if (close(fd2) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
    if (close(fd3) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
