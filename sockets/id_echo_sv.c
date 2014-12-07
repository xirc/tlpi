/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2014.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/
/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
* See above.                                                              *
\*************************************************************************/


/* id_echo_sv.c

   This program implements a daemon that provides the UDP "echo" service. It
   reads datagrams and then sends copies back to the originating address.

   NOTE: this program must be run under a root login, in order to allow the
   "echo" port (7) to be bound. Alternatively, for test purposes, you can edit
   id_echo.h and replace the SERVICE name with a suitable unreserved port
   number (e.g., "51000"), and make a corresponding change in the client.

   See also id_echo_cl.c.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include "id_echo.h"
#include "inet_sockets.h"


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int sfd;
    struct sockaddr_storage claddr;
    socklen_t len;
    ssize_t num_read;
    char buf[BUF_SIZE];
    char addrstr[IS_ADDR_STR_LEN];

    if (daemon(0, 0) == -1) {
        perror("daemon");
        exit(EXIT_FAILURE);
    }

    sfd = inet_bind(SERVICE, SOCK_DGRAM, NULL);
    if (sfd == -1) {
        syslog(LOG_ERR, "Could not create server socket (%s)",
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Receive datagrams and return copies to senders */
    while (1) {
        len = sizeof(struct sockaddr_storage);
        num_read = recvfrom(sfd, buf, BUF_SIZE, 0,
                (struct sockaddr *) &claddr, &len);
        if (num_read == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        if (sendto(sfd, buf, num_read, 0,
                    (struct sockaddr *) &claddr, len) != num_read)
        {
            syslog(LOG_WARNING, "Error echoing response to %s (%s)",
                    inet_addrstr((struct sockaddr *) &claddr, len,
                        addrstr, IS_ADDR_STR_LEN), strerror(errno));
        }
    }

    exit(EXIT_FAILURE);
}
