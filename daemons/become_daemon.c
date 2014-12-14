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


/* become_daemon.c

   A function encapsulating the steps in becoming a daemon.
*/


#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>

#include "become_daemon.h"


/* Return 0 on success, -1 on error */
int
become_daemon(int flags)
{
    int maxfd, fd;

    /* Become background process */
    switch (fork()) {
    case -1:
        return -1;
    case 0:
        /* Child falls through... */
        break;
    default:
        /* while parent terminates */
        _exit(EXIT_SUCCESS);
    }

    /* Become leader of new session */
    if (setsid() == -1) {
        return -1;
    }

    /* Ensure we are not session leader */
    switch (fork()) {
        case -1:
            return -1;
        case 0:
            break;
        default:
            _exit(EXIT_SUCCESS);
    }

    if (!(flags & BD_NO_UMASK0)) {
        /* Clear file mode creation mask */
        umask(0);
    }

    if (!(flags & BD_NO_CHDIR)) {
        /* Change to root directory */
        chdir("/");
    }

    if (!(flags & BD_NO_CLOSE_FILES)) {
        /* Close all open files */
        maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd == -1) {  /* Limit is indeterminate */
            maxfd = BD_MAX_CLOSE;   /* take a guess */
        }

        for (fd = 0; fd < maxfd; ++fd) {
            close(fd);
        }
    }

    if (!(flags & BD_NO_REOPEN_STD_FDS)) {
        close(STDIN_FILENO);        /* Reopen standard fd's to /dev/null */

        fd = open("/dev/null", O_RDWR);
        if (fd != STDIN_FILENO) {
            /* 'fd' should be 0 */
            return -1;
        }
        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO) {
            return -1;
        }
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO) {
            return -1;
        }
    }

    return 0;
}
