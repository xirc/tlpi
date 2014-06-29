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


/* view_symlink.c

   Demonstrate the use of readlink() and realpath() to read and display
   the contents of a symbolic link.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>

#define BUF_MAX PATH_MAX


int
main(int argc, char *argv[])
{
    struct stat statbuf;
    char buf[BUF_MAX];
    ssize_t num_bytes;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (lstat(argv[1], &statbuf) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    if (!S_ISLNK(statbuf.st_mode)) {
        fprintf(stderr, "%s is not a symbolic link\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    num_bytes = readlink(argv[1], buf, BUF_MAX - 1);
    if (num_bytes == -1) {
        perror("readlink");
        exit(EXIT_FAILURE);
    }
    buf[num_bytes] = '\0';
    printf("readlink: %s --> %s\n", argv[1], buf);

    if (realpath(argv[1], buf) == NULL) {
        perror("realpath");
        exit(EXIT_FAILURE);
    }
    printf("realpath: %s --> %s\n", argv[1], buf);

    exit(EXIT_SUCCESS);
}
