/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int
main(int argc, char *argv[])
{
    int i, num_count, flag;
    int fd1, fd2;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s num-count [flag] \n", argv[0]);
        fprintf(stderr, "flag: '1' -> use fchdir\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    num_count = atoi(argv[1]);
    if (num_count <= 0) {
        fprintf(stderr, "num-count %s > 0\n", argv[2]);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
    flag = (argc > 2);

    printf("use %s\n", flag ? "fchdir" : "chdir");
    if (flag) {
        fd1 = open("/", O_RDONLY);
        if (fd1 == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        fd2 = open("/tmp", O_RDONLY);
        if (fd2 == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
    }
    for (i = 0; i < num_count; ++i) {
        if (flag) {
            fchdir(fd1);
            fchdir(fd2);
        } else {
            chdir("/");
            chdir("/tmp");
        }
    }

    exit(EXIT_SUCCESS);
}
