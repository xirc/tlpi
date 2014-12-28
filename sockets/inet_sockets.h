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


/* inet_sockets.h

   Header file for inet_sockets.c.
*/


#ifndef INET_SOCKETS_H
#define INET_SOCKETS_H


#include <sys/socket.h>
#include <netdb.h>

int inet_connect(const char *host, const char *service, int type);
int inet_listen(const char *service, int backlog, socklen_t *addrlen);
int inet_bind(const char *service, int type, socklen_t *addrlen);
char* inet_addrstr(const struct sockaddr *addr, socklen_t addrlen,
        char *addrstr, int addrstr_len);

#define IS_ADDR_STR_LEN 4096
    /* Suggested length for string buffer that caller
     * should pass to inet_addrstr(). Must be greater
     * than (NI_MAXHOST + NI_MAXSERV + 4)
     */


#endif
