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


/* socknames.c

   Demonstrate the use of getsockname() and getpeername() to retrieve the local
   and peer socket addresses.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "inet_sockets.h"


int
main(int argc, char *argv[])
{
    int lfd, afd, cfd;
    socklen_t len;      /* Size of socket address buffer */
    void *addr;         /* Buffer for socket address */
    char addrstr[IS_ADDR_STR_LEN];

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s service\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    lfd = inet_listen(argv[1], 5, &len);
    if (lfd == -1) {
        perror("inet_listen");
        exit(EXIT_FAILURE);
    }

    cfd = inet_connect(NULL, argv[1], SOCK_STREAM);
    if (cfd == -1) {
        perror("inet_connect");
        exit(EXIT_FAILURE);
    }

    afd = accept(lfd, NULL, NULL);
    if (afd == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    addr = malloc(len);
    if (addr == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if (getsockname(cfd, addr, &len) == -1) {
        perror("getsockname");
        exit(EXIT_FAILURE);
    }
    printf("getsockname(cfd):   %s\n",
            inet_addrstr(addr, len, addrstr, IS_ADDR_STR_LEN));
    if (getsockname(afd, addr, &len) == -1) {
        perror("getsockname");
        exit(EXIT_FAILURE);
    }
    printf("getsockname(afd):   %s\n",
            inet_addrstr(addr, len, addrstr, IS_ADDR_STR_LEN));

    if (getpeername(cfd, addr, &len) == -1) {
        perror("getpeername");
        exit(EXIT_FAILURE);
    }
    printf("getpeername(cfd):   %s\n",
            inet_addrstr(addr, len, addrstr, IS_ADDR_STR_LEN));
    if (getpeername(afd, addr, &len) == -1) {
        perror("getpeername");
        exit(EXIT_FAILURE);
    }
    printf("getpeername(afd):  %s\n",
            inet_addrstr(addr, len, addrstr, IS_ADDR_STR_LEN));

    /* Give us time to run netstat(8) */
    sleep(30);

    exit(EXIT_SUCCESS);
}
