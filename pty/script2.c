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


/* script.c

   A simple version of script(1).
*/


#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "pty_fork.h"
#include "tty_functions.h"

#define BUF_SIZE       256
#define MAX_SLNAME    1024


struct termios tty_orig;
struct winsize ws_orig;

/* Reset terminal mode on program exit */
static void
tty_reset(void)
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty_orig) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}


static sig_atomic_t got_sigwinch = 0;
static void
sigwinch_handler(int sig __attribute__((unused)))
{
    got_sigwinch = 1;
}


static int
timestamp(int fd, const char *prefix, const struct tm *tm)
{
    char timestr[1024];

    time_t t;
    if (tm == NULL) {
        t = time(NULL);
        tm = localtime(&t);
    }

    strftime(timestr, sizeof(timestr), "%F %T", tm);
    if (write(fd, prefix, strlen(prefix)) == -1) {
        return -1;
    }
    if (write(fd, timestr, strlen(timestr)) == -1) {
        return -1;
    }
    if (write(fd, "\n", 1) == -1) {
        return -1;
    }

    return 0;
}


int
main(int argc, char *argv[])
{
    char slave_name[MAX_SLNAME];
    char *shell;
    int master_fd, script_fd;
    struct winsize ws;
    fd_set infds;
    char buf[BUF_SIZE];
    ssize_t num_reads;
    pid_t child_pid;
    struct sigaction sa;

    if (tcgetattr(STDIN_FILENO, &tty_orig) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws_orig) < 0) {
        perror("ioctl-TIOCGWINSZ");
        exit(EXIT_FAILURE);
    }

    child_pid = pty_fork(&master_fd, slave_name, MAX_SLNAME, &tty_orig, &ws);
    if (child_pid == -1) {
        perror("ptty_fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        /* Child: execute a shell on pty slave */
        shell = getenv("SHELL");
        if (shell == NULL || *shell == '\0') {
            shell = "/bin/sh";
        }
        execlp(shell, shell, (char*) NULL);
        perror("execlp");       /* If we get here, something went wrong */
        exit(EXIT_FAILURE);
    }

    /* Parent: relay data between terminal and pty master */
    script_fd = open((argc > 1) ? argv[1] : "typescript",
            O_WRONLY | O_CREAT | O_TRUNC,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (script_fd == -1) {
        perror("open typescript");
        exit(EXIT_FAILURE);
    }

    tty_set_raw(STDIN_FILENO, &tty_orig);
    if (atexit(tty_reset) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigwinch_handler;
    if (sigaction(SIGWINCH, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Record start time stamp */
    if (timestamp(script_fd, "start # ", NULL) == -1) {
        perror("timestamp");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (got_sigwinch) {
            got_sigwinch = 0;

            if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0) {
                perror("ioctl-TIOCGWINSZ");
                exit(EXIT_FAILURE);
            }
            if (ioctl(master_fd, TIOCSWINSZ, &ws) < 0) {
                perror("ioctl-TIOCSWINSZ");
                exit(EXIT_FAILURE);
            }
        }

        FD_ZERO(&infds);
        FD_SET(STDIN_FILENO, &infds);
        FD_SET(master_fd, &infds);

        if (select(master_fd + 1, &infds, NULL, NULL, NULL) == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(STDIN_FILENO, &infds)) {
            /* stdin --> pty */
            num_reads = read(STDIN_FILENO, buf, BUF_SIZE);
            if (num_reads <= 0) {
                (void) timestamp(script_fd, "end # ", NULL);
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
                (void) timestamp(script_fd, "end # ", NULL);
                exit(EXIT_SUCCESS);
            }
            if (write(STDOUT_FILENO, buf, num_reads) != num_reads) {
                fprintf(stderr, "fatal partial/failed write (STDOUT_FILENO)\n");
                exit(EXIT_FAILURE);
            }
            if (write(script_fd, buf, num_reads) != num_reads) {
                fprintf(stderr, "fatal partial/failed wirte (scriptfd)\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    exit(EXIT_SUCCESS);
}
