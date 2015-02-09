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


/* unbuffer.c

   Use a pseudoterminal to circumvent the block buffering performed by the
   stdio library when standard output is redirected to a file or pipe.
   When a program is started using 'unbuffer', its output is redirected to
   a pseudoterminal, and is consequently line-buffered.

   Usage: unbuffer prog [arg ...]
*/


#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "pty_fork.h"
#include "tty_functions.h"

#define BUF_SIZE       256
#define MAX_SLNAME    1024


struct termios tty_orig;

/* Reset terminal mode on program exit */
static void
tty_reset(void)
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty_orig) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}


int
main(int argc, char *argv[])
{
    char slave_name[MAX_SLNAME];
    int master_fd;
    struct winsize ws;
    fd_set infds;
    char buf[BUF_SIZE];
    ssize_t num_reads;
    pid_t child_pid;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s cmd...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (tcgetattr(STDIN_FILENO, &tty_orig) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0) {
        perror("ioctl-TIOCGWINSZ");
        exit(EXIT_FAILURE);
    }

    child_pid = pty_fork(&master_fd, slave_name, MAX_SLNAME, &tty_orig, &ws);
    if (child_pid == -1) {
        perror("ptty_fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        /* Child: execute a argv[1] on pty slave */
        execvp(argv[1], argv+1);
        perror("execlp");       /* If we get here, something went wrong */
        exit(EXIT_FAILURE);
    }

    /* Parent: relay data between terminal and pty master */
    tty_set_raw(STDIN_FILENO, &tty_orig);
    if (atexit(tty_reset) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    while (1) {
        FD_ZERO(&infds);
        FD_SET(STDIN_FILENO, &infds);
        FD_SET(master_fd, &infds);

        if (select(master_fd + 1, &infds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(STDIN_FILENO, &infds)) {
            /* stdin --> pty */
            num_reads = read(STDIN_FILENO, buf, BUF_SIZE);
            if (num_reads <= 0) {
                exit(EXIT_SUCCESS);
            }
            if (write(master_fd, buf, num_reads) != num_reads) {
                fprintf(stderr, "fatal partial/failed write (masterfd)\n");
                exit(EXIT_FAILURE);
            }
        }

        if (FD_ISSET(master_fd, &infds)) {
            /* pty --> stdout + file */
            num_reads = read(master_fd, buf, BUF_SIZE);
            if (num_reads <= 0) {
                exit(EXIT_SUCCESS);
            }
            if (write(STDOUT_FILENO, buf, num_reads) != num_reads) {
                fprintf(stderr, "fatal partial/failed write (STDOUT_FILENO)\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    exit(EXIT_SUCCESS);
}
