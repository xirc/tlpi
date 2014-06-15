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
#include <fcntl.h>

#include "mdup.h"


int
mdup(int oldfd)
{
    return fcntl(oldfd, F_DUPFD, 0);
}


int
mdup2(int oldfd, int newfd)
{
    if (fcntl(oldfd, F_GETFL) == -1) {
        errno = EBADF;
        return -1;
    }
    if (oldfd == newfd) {
        return newfd;
    }
    if (close(newfd) == -1) {
        /* do nothing, ignore! */
    }
    if (fcntl(oldfd, F_DUPFD, newfd) == -1) {
        return -1;
    }
    return newfd;
}
