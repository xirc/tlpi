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


/* pty_fork.c

   Implements pty_fork(), a function that creates a child process connected to
   the parent (i.e., the calling process) via a pseudoterminal (pty). The child
   is placed in a new session, with the pty slave as its controlling terminal,
   and its standard input, output, and error connected to the pty slave.

   In the parent, 'master_fd' is used to return the file descriptor for the
   pty master.

   If 'slave_same' is non-NULL, then it is used to return the name of the pty
   slave. If 'slave_name' is not NULL,  then 'snlen' should be set to indicate
   the size of the buffer pointed to by 'slave_name'.

   If 'slave_termios' and 'slave_ws' are non-NULL, then they are used respectively
   to set the terminal attributes and window size of the pty slave.

   Returns:
        in child: 0
        in parent: PID of child or -1 on error
*/


#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "pty_master_open.h"
#include "pty_fork.h"


#define MAX_SLNAME PATH_MAX


pid_t
pty_fork(int *master_fd, char *slave_name, size_t snlen,
         const struct termios *slave_termios, const struct winsize *slave_ws)
{
    int mfd, sfd, saved_errno;
    pid_t cpid;
    char slname[MAX_SLNAME];

    mfd = pty_master_open(slname, MAX_SLNAME);
    if (mfd == -1) {
        return -1;
    }

    /* Return slave name to caller */
    if (slave_name != NULL) {
        if (strlen(slname) < snlen) {
            strncpy(slave_name, slname, snlen);
        } else {
            /* 'slave_name' was too small */
            (void) close(mfd);
            errno = EOVERFLOW;
            return -1;
        }
    }

    cpid = fork();
    if (cpid == -1) {
        /* fork failed */
        /* close() might be change 'errno' */
        saved_errno = errno;
        (void) close(mfd);
        errno = saved_errno;
        return -1;
    }

    if (cpid != 0) {
        /* Parent */
        /* Only parent gets master fd and child PID */
        *master_fd = mfd;
        return cpid;
    }

    /* Child falls through to here */
    if (setsid() == -1) {
        /* Start a new session */
        perror("pty_fork:setsid");
        _exit(EXIT_FAILURE);
    }

    (void) close(mfd);              /* Not needed in child */
    sfd = open(slname, O_RDWR);     /* Becomes controlling tty */
    if (sfd == -1) {
        perror("pty_fork:open-slave");
        _exit(EXIT_FAILURE);
    }

#ifdef TIOCSCTTY        /* Acquire controlling tty on BSD */
    if (ioctl(sfd, TIOCSCTTY, 0) == -1) {
        perror("pty_fork:ioctl-TIOCSCTTY");
        _exit(EXIT_FAILURE);
    }
#endif

    if (slave_termios != NULL) {
        /* Set slave tty attributes */
        if (tcsetattr(sfd, TCSANOW, slave_termios) == -1) {
            perror("pty_fork:tcsetattr");
            _exit(EXIT_FAILURE);
        }
    }

    if (slave_ws != NULL) {
        /* Set slave tty window size */
        if (ioctl(sfd, TIOCSWINSZ, slave_ws) == -1) {
            perror("pty_fork:ioctl-TIOCSWINSZ");
            _exit(EXIT_FAILURE);
        }
    }

    /* Duplicate pty slave to be child's stdin, stdout, and stderr */
    if (dup2(sfd, STDIN_FILENO) != STDIN_FILENO) {
        perror("pty_fork:dup2-STDIN_FILENO");
        _exit(EXIT_FAILURE);
    }
    if (dup2(sfd, STDOUT_FILENO) != STDOUT_FILENO) {
        perror("pty_fork:dup2-STDOUT_FILENO");
        _exit(EXIT_FAILURE);
    }
    if (dup2(sfd, STDERR_FILENO) != STDERR_FILENO) {
        perror("pty_fork:dup2-STDOUT_FILENO");
        _exit(EXIT_FAILURE);
    }

    if (sfd > STDERR_FILENO) { /*Safety check */
        (void) close(sfd);     /* No longer need this fd */
    }

    return 0;
}
