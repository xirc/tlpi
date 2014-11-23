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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>


#define NLOOPS 10000


int
main(int argc, char *argv[])
{
    int i;
    int opt_start;
    int fd;
    struct flock fl;

    if (argc < 3) {
        fprintf(stderr, "%s file N\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    opt_start = strtoul(argv[2], NULL, 0);

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = opt_start * 2;
    fl.l_len = 1;
    for (i = 0; i < NLOOPS; ++i) {
        (void) fcntl(fd, F_SETLK, &fl);
    }

    exit(EXIT_SUCCESS);
}
