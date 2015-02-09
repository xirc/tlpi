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
#include <limits.h>
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
#define RECORD_SIZE   8096


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


static long
diff_timespec_ms(const struct timespec *begin, const struct timespec *end)
{
    long diff_ms;

    diff_ms = 0;
    diff_ms += ((long) end->tv_sec - begin->tv_sec) * 1000;
    diff_ms += ((long) end->tv_nsec - begin->tv_nsec) / 1000000;

    return diff_ms;
}


static ssize_t
make_record(unsigned long timestamp, const char *str, size_t len,
            char *buf, size_t buflen)
{
    int size, rem;
    char *p;
    size_t j;

    p = buf;
    rem = buflen;
    size = snprintf(p, rem, "%lu ", timestamp);
    if (size > (long) buflen) {
        errno = EOVERFLOW;
        return -1;
    }
    p += size;
    rem -= size;

    for (j = 0; j < len && rem > 1; ++j) {
        if (str[j] == '\n') {
            rem -= 2;
            if (rem < 1) {
                break;
            }
            *p++ = '\\';
            *p++ = 'n';
        } else {
            --rem;
            *p++ = str[j];
        }
    }
    if (rem < 1) {
        errno = EOVERFLOW;
        return -1;
    }
    *p = '\0';

    return buflen - rem;
}


static int
write_record(int fd, unsigned long timestamp, const char *str, size_t len)
{
    char buf[RECORD_SIZE];
    ssize_t reclen;

    reclen = make_record(timestamp, str, len, buf, RECORD_SIZE);
    if (reclen == -1) {
        return -1;
    }

    if (write(fd, buf, reclen) == -1) {
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
    int master_fd, script_fd, script_timed_fd;
    mode_t scriptfile_mode;
    char script_name[PATH_MAX];
    char script_timed_name[PATH_MAX];
    struct winsize ws;
    fd_set infds;
    char buf[BUF_SIZE];
    ssize_t num_reads;
    pid_t child_pid;
    struct timespec begin, now;
    long elapsed_ms;

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
    snprintf(script_name, PATH_MAX, "%s", (argc > 1) ? argv[1] : "typescript");
    snprintf(script_timed_name, PATH_MAX, "%s.timed", script_name);
    scriptfile_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    script_fd = open(script_name, O_WRONLY | O_CREAT | O_TRUNC, scriptfile_mode);
    if (script_fd == -1) {
        perror("open typescript");
        exit(EXIT_FAILURE);
    }
    script_timed_fd = open(script_timed_name,
            O_WRONLY | O_CREAT | O_TRUNC, scriptfile_mode);
    if (script_timed_fd == -1) {
        perror("open typescript.timed");
        exit(EXIT_FAILURE);
    }

    tty_set_raw(STDIN_FILENO, &tty_orig);
    if (atexit(tty_reset) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    if (clock_gettime(CLOCK_MONOTONIC_RAW, &begin) == -1) {
        perror("clock_gettime");
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
            if (write(script_fd, buf, num_reads) != num_reads) {
                fprintf(stderr, "fatal partial/failed wirte (scriptfd)\n");
                exit(EXIT_FAILURE);
            }
            if (clock_gettime(CLOCK_MONOTONIC_RAW, &now) == -1) {
                perror("clock_gettime");
                exit(EXIT_FAILURE);
            }
            elapsed_ms = diff_timespec_ms(&begin, &now);
            if (write_record(script_timed_fd, elapsed_ms, buf, num_reads) == -1) {
                perror("write_record");
                exit(EXIT_FAILURE);
            }
        }
    }

    exit(EXIT_SUCCESS);
}
