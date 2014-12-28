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


/* region_locking.c

   Some useful functions for file region (fcntl()) locking.
*/


#include <fcntl.h>
#include "region_locking.h"


/* Lock a file region (private; public interfaces below) */
static int
lockreg(int fd, int cmd, int type, int whence, int start, off_t len)
{
    struct flock fl;

    fl.l_type = type;
    fl.l_whence = whence;
    fl.l_start = start;
    fl.l_len = len;

    return fcntl(fd, cmd, &fl);
}


int
lock_region(int fd, int type, int whence, int start, int len)
{
    return lockreg(fd, F_SETLK, type, whence, start, len);
}


int
lock_region_wait(int fd, int type, int whence, int start, int len)
{
    return lockreg(fd, F_SETLKW, type, whence, start, len);
}


pid_t
is_region_locked(int fd, int type, int whence, int start, int len)
{
    struct flock fl;

    fl.l_type = type;
    fl.l_whence = whence;
    fl.l_start = start;
    fl.l_len = len;

    if (fcntl(fd, F_GETLK, &fl) == -1) {
        return -1;
    }

    return (fl.l_type == F_UNLCK) ? 0 : fl.l_pid;
}
