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
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <termios.h>

#include "pty_fork.h"


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int mfd;
    pid_t cpid;
    char slave_name[PATH_MAX];
    struct termios termios;
    struct winsize winsize;

    if (tcgetattr(STDIN_FILENO, &termios) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    if (ioctl(STDIN_FILENO, TIOCSWINSZ, &winsize) == -1) {
        perror("ioctl-TIOCSWINSZ");
        exit(EXIT_FAILURE);
    }

    cpid = pty_fork(&mfd, slave_name, PATH_MAX, &termios, &winsize);
    if (cpid == -1) {
        perror("pty_fork");
        exit(EXIT_FAILURE);
    }

    switch (cpid) {
    case 0:  /* Child */
        exit(EXIT_SUCCESS);
        break;
    default: /* Parent */
        printf("PARENT: mfd: %d, cpid: %d, slave pty name: %s\n",
                mfd, cpid, slave_name);
        if (waitpid(-1, NULL, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        break;
    }

    exit(EXIT_SUCCESS);
}
