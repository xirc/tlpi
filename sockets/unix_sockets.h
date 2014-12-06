/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#ifndef INET_SOCKETS_H
#define INET_SOCKETS_H


#include <sys/un.h>
#include <sys/socket.h>


int unix_connect(const char *path, int type);
int unix_listen(const char *path, int backlog);
int unix_bind(const char *path, int type);
char* unix_addrstr(const struct sockaddr_un *addr,
        char *addrstr, size_t addrstr_len);


#endif
