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
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "us_seqnum.h"


int
main(int argc, char *argv[])
{
    int sfd;
    struct sockaddr_un addr;
    struct request req;
    struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s [seq-len...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Create client socket */
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Construct request message */
    req.seq_len = (argc > 1) ? atoi(argv[1]) : 1;
    if (req.seq_len <= 0) {
        fprintf(stderr, "seq-len '%s' > 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Build server address, and make the connection */
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, US_SERVER_PATH, sizeof(addr.sun_path) - 1);
    if (connect(sfd, (struct sockaddr *) &addr,
                sizeof(struct sockaddr_un)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    /* Send request */
    if (send(sfd, &req, sizeof(struct request), 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    /* Receive response */
    if (recv(sfd, &resp, sizeof(struct response), 0) == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", resp.seq_num);

    exit(EXIT_SUCCESS);
}
