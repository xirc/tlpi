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
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


#define BUF_SIZE 1024
#define SLEEP_TIME 3


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    char tty_path[L_ctermid];
    int ttyfd;
    pid_t pid;
    int rc;
    char buf[BUF_SIZE];

    if (ctermid(tty_path) == NULL) {
        perror("ctermid");
        exit(EXIT_FAILURE);
    }
    ttyfd = open(tty_path, O_RDWR);
    if (ttyfd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    printf("This program fork and the child exit after about %d secs\n",
            SLEEP_TIME);

    pid = fork();
    switch (pid) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:  /* Child */
        /* Make parent a chance to exit. */
        sleep(SLEEP_TIME);
        rc = read(ttyfd, buf, BUF_SIZE);
        if (rc == -1 && errno == EIO) {
            printf("Child faild to read tty (expected behaviour)\n");
        } else {
            fprintf(stderr, "Child faild or success"
                    "to read tty (unexpected behaviour)\n");
            fprintf(stderr, "return code = %d, errno = %d (%s)\n",
                    rc, errno, strerror(errno));
        }
        exit(EXIT_SUCCESS);
    default: /* Parent */
        /* Make child orphan process */
        exit(EXIT_SUCCESS);
    }

    /* cannot reach here */
    exit(EXIT_SUCCESS);
}
