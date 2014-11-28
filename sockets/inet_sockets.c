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


#define _BSD_SOURCE
    /* To get NI_MAXHOST and NI_MAXSERV
     * definitions from <netdb.h>
     */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>

#include "inet_sockets.h"


int
inet_connect(const char *host, const char *service, int type)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;    /* Allows IPv4 or IPv6 */
    hints.ai_socktype = type;

    s = getaddrinfo(host, service, &hints, &result);
    if (s != 0) {
        errno = ENOSYS;
        return -1;
    }

    /* Walk through returned list until we find an address structure
     * that can be used to successfully connect a socket
     */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            /* On error, try next address */
            continue;
        }

        if (connect(sfd,  rp->ai_addr, rp->ai_addrlen) != -1) {
            /* Success */
            break;
        }

        /* connect() failed: close this socket and try next address */
        (void) close(sfd);
    }

    freeaddrinfo(result);

    return (rp == NULL) ? -1 : sfd;
}


/* Public interfaces: inet_bind() and inet_listen() */
static int
inet_passive_socket(const char *service, int type, socklen_t *addrlen,
        int do_listen, int backlog)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, optval, s;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = type;
    hints.ai_family = AF_UNSPEC;        /* Allows IPv4 or IPv6 */
    hints.ai_flags = AI_PASSIVE;        /* Use wildcard IP address */

    s = getaddrinfo(NULL, service, &hints, &result);
    if (s != 0) {
        return -1;
    }

    /* Walk through returned list until we find an address structure
     * that can be used to successfully create and bind a socket
     */
    optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            /* On error, try next address */
            continue;
        }

        if (do_listen) {
            if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR,
                        &optval, sizeof(optval)) == -1)
            {
                (void) close(sfd);
                freeaddrinfo(result);
                return -1;
            }
        }

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            /* Success */
            break;
        }

        /* bind() failed: close this socket and try next address */
        (void) close(sfd);
    }

    if (rp != NULL && do_listen) {
        if (listen(sfd, backlog) == -1) {
            (void) close(sfd);
            freeaddrinfo(result);
            return -1;
        }
    }

    if (rp != NULL && addrlen != NULL) {
        *addrlen = rp->ai_addrlen;  /* Return address structure size */
    }

    freeaddrinfo(result);

    return (rp == NULL) ? -1 : sfd;
}


int
inet_listen(const char *service, int backlog, socklen_t *addrlen)
{
    return inet_passive_socket(service, SOCK_STREAM,
            addrlen, /* True */ 1, backlog);
}


int
inet_bind(const char *service, int type, socklen_t *addrlen)
{
    return inet_passive_socket(service, type,
            addrlen, /* False */ 0, 0);
}


char *
inet_addrstr(const struct sockaddr *addr, socklen_t addrlen,
        char *addrstr, int addrstr_len)
{
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    if (getnameinfo(addr, addrlen, host, NI_MAXHOST,
                service, NI_MAXSERV, NI_NUMERICSERV) == 0)
    {
        snprintf(addrstr, addrstr_len, "(%s %s)", host, service);
    } else {
        snprintf(addrstr, addrstr_len, "(?UNKNOWN?)");
    }
    return addrstr;
}
