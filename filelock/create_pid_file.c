/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2014.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/
/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
* See above.                                                              *
\*************************************************************************/


/* create_pid_file.c

   Implement a function that can be used by a daemon (or indeed any program)
   to ensure that only one instance of the program is running.
*/


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "region_locking.h"
#include "create_pid_file.h"


/* Large enough to hold maximum PID as string */
#define BUF_SIZE 100


int
create_pid_file(const char *prog_name, const char *pid_file, int flags)
{
    int fd;
    char buf[BUF_SIZE];

    fd = open(pid_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        fprintf(stderr, "Could not open PID file %s: ", pid_file);
        perror("");
        exit(EXIT_FAILURE);
    }

    if (flags & CPF_CLOEXEC) {
        /* Set the close-on-exec file descriptor flag */
        flags = fcntl(fd, F_GETFD);
        if (flags == -1) {
            fprintf(stderr,
                    "Could not get flags for PID file %s: ", pid_file);
            perror("");
            exit(EXIT_FAILURE);
        }
        flags |= FD_CLOEXEC;
        if (fcntl(fd, F_SETFD, flags) == -1) {
            fprintf(stderr,
                    "Could not set flags for PID file %s: ", pid_file);
            perror("");
            exit(EXIT_FAILURE);
        }
    }

    if (lock_region(fd, F_WRLCK, SEEK_SET, 0, 0) == -1) {
        if (errno == EAGAIN || errno == EACCES) {
            fprintf(stderr, "PID file '%s' is locked; "
                    "probably '%s' is already running\n",
                    pid_file, prog_name);
            exit(EXIT_FAILURE);
        } else {
            fprintf(stderr, "Unable to lock PID file '%s': ", pid_file);
            perror("");
            exit(EXIT_FAILURE);
        }
    }

    if (ftruncate(fd, 0) == -1) {
        fprintf(stderr, "Could not truncate PID file '%s': ", pid_file);
        perror("");
        exit(EXIT_FAILURE);
    }

    snprintf(buf, BUF_SIZE, "%ld\n", (long) getpid());
    if (write(fd, buf, strlen(buf)) != (ssize_t)strlen(buf)) {
        fprintf(stderr, "Writing to PID file '%s': ", pid_file);
        perror("");
        exit(EXIT_FAILURE);
    }

    return fd;
}
