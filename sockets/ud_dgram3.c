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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 4096
#define SOCK_PATH_A "/tmp/a"
#define SOCK_PATH_B "/tmp/b"
#define SOCK_PATH_C "/tmp/c"


static int
make_socket(struct sockaddr_un *addr, const char *path)
{
    int sfd;

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (remove(path) == -1 && errno != ENOENT) {
        fprintf(stderr, "remvoe %s: %s\n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    memset(addr, 0, sizeof(struct sockaddr_un));
    addr->sun_family = AF_UNIX;
    strncpy(addr->sun_path, path, sizeof(addr->sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) addr,
                sizeof(struct sockaddr_un)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    return sfd;
}


static void
remove_socket(int sfd, const char *path)
{
    (void) close(sfd);
    (void) remove(path);
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct sockaddr_un addr_a, addr_b, addr_c;
    int sfda, sfdb, sfdc;
    char buf[BUF_SIZE];

    sfda = make_socket(&addr_a, SOCK_PATH_A);
    sfdb = make_socket(&addr_b, SOCK_PATH_B);
    sfdc = make_socket(&addr_c, SOCK_PATH_C);

    /* A --> B */
    if (connect(sfda, (struct sockaddr *) &addr_b,
                sizeof(struct sockaddr_un)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    /* C --> B */
    /* ERROR or not ? */
    if (sendto(sfdc, buf, BUF_SIZE, 0, (struct sockaddr *) &addr_a,
                sizeof(struct sockaddr)) == -1)
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    } else {
        printf("sendto %s -> %s\n", addr_c.sun_path, addr_a.sun_path);
    }


    remove_socket(sfda, SOCK_PATH_A);
    remove_socket(sfdb, SOCK_PATH_B);
    remove_socket(sfdc, SOCK_PATH_C);

    exit(EXIT_SUCCESS);
}
