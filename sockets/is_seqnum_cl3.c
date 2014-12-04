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


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>

#include "is_seqnum.h"
#include "readline2.h"
#include "inet_sockets.h"

#define BUFSIZE 4096


int
main(int argc, char *argv[])
{
    char *reqlen_str;           /* Requested length of sequence */
    char seqnum_str[INT_LEN];    /* Start of granted sequence */
    int cfd;
    ssize_t num_read;
    struct rlbuf rlbuf;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s server-host [sequence-len]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    cfd = inet_connect(argv[1], PORT_NUM, SOCK_STREAM);
    if (cfd == -1) {
        perror("inet_bind");
        exit(EXIT_FAILURE);
    }

    /* Send requested sequence length, with terminating newline */
    reqlen_str = (argc > 2) ? argv[2] : "1";
    if (write(cfd, reqlen_str, strlen(reqlen_str)) !=
            (ssize_t) strlen(reqlen_str))
    {
        fprintf(stderr, "Partial/failed write (reqlen_str)\n");
        exit(EXIT_FAILURE);
    }
    if (write(cfd, "\n", 1) != 1) {
        fprintf(stderr, "Partial/failed write (newline)\n");
        exit(EXIT_FAILURE);
    }

    /* Read and display sequence number returned by server */
    if (readline_init(&rlbuf, cfd, BUFSIZE) == -1) {
        perror("readline_init");
        exit(EXIT_FAILURE);
    }
    num_read = readline(&rlbuf, seqnum_str, INT_LEN);
    if (num_read == -1) {
        perror("read_line");
        exit(EXIT_FAILURE);
    }
    if (num_read == 0) {
        fprintf(stderr, "Unexpected EOF from server\n");
        exit(EXIT_FAILURE);
    }
    readline_free(&rlbuf);

    printf("Sequence number: %s", seqnum_str);  /* Includes '\n' */

    (void) close(cfd);
    exit(EXIT_SUCCESS);
}
