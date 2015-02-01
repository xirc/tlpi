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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>


static int
m_isatty(int fd)
{
    int retc;
    struct termios t;

    retc = tcgetattr(fd, &t);
    return (retc == 0);
}


int
main(int argc, char *argv[])
{
    int i, fd;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [file]...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (i = 1; i < argc; ++i) {
        fd = open(argv[i], O_RDONLY);
        if (fd == -1) {
            continue;
        }
        printf("isatty:   %s %s\n", argv[i], isatty(fd) == 1 ? "TTY" : "NOTTY");
        printf("m_isatty: %s %s\n", argv[i], m_isatty(fd) == 1 ? "TTY" : "NOTTY");
    }

    exit(EXIT_SUCCESS);
}
