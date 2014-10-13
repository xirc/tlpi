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
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>


static char fifopath[PATH_MAX];


static void
remove_fifo(void)
{
    (void) unlink(fifopath);
}


#define FIFO_PATH_TEMPLATE "/tmp/m_fifo_nonblocking.fifo"
int
main(int argc, char *argv[])
{
    int fifofd, flags;
    int is_nonblocking;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s {r|w} [nonblocking]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (strchr("rw", argv[1][0]) == NULL) {
        fprintf(stderr, "ARGUMENT#1 should be 'r' or 'w'\n");
        exit(EXIT_FAILURE);
    }
    is_nonblocking = (argc > 2) ? 1 : 0;

    /* Make FIFO and set atexit */
    snprintf(fifopath, PATH_MAX, FIFO_PATH_TEMPLATE);
    umask(0);
    if (mkfifo(fifopath, S_IRUSR | S_IWUSR | S_IRGRP) == -1 &&
        errno != EEXIST)
    {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
    atexit(remove_fifo);

    /* read or write */
    flags = 0;
    switch (argv[1][0]) {
    case 'r': flags |= O_RDONLY; break;
    case 'w': flags |= O_WRONLY; break;
    default: exit(EXIT_FAILURE);
    }
    if (is_nonblocking) {
        flags |= O_NONBLOCK;
    }
    errno = 0;
    fifofd = open(fifopath, flags);
    printf("%d (errno=%d, errmsg=%s)\n", fifofd, errno, strerror(errno));
    close(fifofd);

    exit(EXIT_SUCCESS);
}
