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


/* pty_master_open.c

   Implement our pty_master_open() function, based on UNIX 98 pseudoterminals.
*/


/* #define _XOPEN_SOURCE 600 */
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pty_master_open.h"


int
pty_master_open(char *slave_name, size_t snlen)
{
    int mfd, saved_errno;
    char *p;

    mfd = posix_openpt(O_RDWR | O_NOCTTY);
        /* Open pty master */
    if (mfd == -1) {
        return -1;
    }

    /* Grant access to slave pty */
    if (grantpt(mfd) == -1) {
        saved_errno = errno;
        (void) close(mfd);      /* Might change 'errno' */
        errno = saved_errno;
        return -1;
    }

    /* Unlock slave pty */
    if (unlockpt(mfd) == -1) {
        saved_errno = errno;
        (void) close(mfd);      /* Might change 'errno' */
        errno = saved_errno;
        return -1;
    }

    /* Get slave pty name */
    p = ptsname(mfd);
    if (p == NULL) {
        saved_errno = errno;
        (void) close(mfd);      /* Might change 'errno' */
        errno = saved_errno;
        return -1;
    }

    if (strlen(p) < snlen) {
        strncpy(slave_name, p, snlen);
    } else {
        /* Return an error if buffer too small */
        (void) close(mfd);
        errno = EOVERFLOW;
        return -1;
    }

    return mfd;
}
