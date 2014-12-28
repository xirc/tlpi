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
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "unix_sockets.h"


int
unix_connect(const char *path, int type)
{
    struct sockaddr_un addr;
    int sfd;

    if (path == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* "\0*****\n" */
    if (strlen(path) > sizeof(addr.sun_path) - 2) {
        errno = EINVAL;
        return -1;
    }

    sfd = socket(AF_UNIX, type, 0);
    if (sfd == -1) {
        return -1;
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path+1, path, sizeof(addr.sun_path) - 1);

    if (connect(sfd, (struct sockaddr *) &addr,
                sizeof(struct sockaddr_un)) == -1)
    {
        return -1;
    }

    return sfd;
}



int
unix_listen(const char *path, int backlog)
{
    int sfd;

    sfd = unix_bind(path, SOCK_STREAM);
    if (sfd == -1) {
        return -1;
    }

    if (listen(sfd, backlog) == -1) {
        return -1;
    }
    return sfd;
}


int
unix_bind(const char *path, int type)
{
    struct sockaddr_un addr;
    int sfd;

    if (path == NULL) {
        errno = EINVAL;
        return -1;
    }
    /* "\0*****\n" */
    if (strlen(path) > sizeof(addr.sun_path) - 2) {
        errno = EINVAL;
        return -1;
    }

    sfd = socket(AF_UNIX, type, 0);
    if (sfd == -1) {
        return -1;
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path+1, path, sizeof(addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &addr,
                sizeof(struct sockaddr_un)) == -1)
    {
        return -1;
    }

    return sfd;
}


char *
unix_addrstr(const struct sockaddr_un *addr,
        char *addrstr, size_t addrstr_len)
{
    const char *path;
    size_t pathsize;

    if (addr == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (addr->sun_family != AF_UNIX) {
        errno = EINVAL;
        return NULL;
    }

    if (addr->sun_path[0] == '\0') {
        path = addr->sun_path + 1;
    } else {
        path = addr->sun_path;
    }
    pathsize = sizeof(path);

    if (addrstr_len < pathsize) {
        errno = EINVAL;
        return NULL;
    }
    snprintf(addrstr, addrstr_len, "%s", path);

    return addrstr;
}
