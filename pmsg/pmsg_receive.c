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


/* pmsg_receive.c

   Usage as shown in usageError().

   Receive a message from a POSIX message queue, and write it on
   standard output.

   See also pmsg_send.c.

   Linux supports POSIX message queues since kernel 2.6.6.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp, "Usage: %s [-n] name\n", progname);
    fprintf(fp, "    -n           Use O_NONBLOCK flag\n");
}


int
main(int argc, char *argv[])
{
    int flags, opt;
    mqd_t mqd;
    unsigned int prio;
    void *buffer;
    struct mq_attr attr;
    ssize_t num_read;

    flags = O_RDONLY;
    while ((opt = getopt(argc, argv, "n")) != -1) {
        switch (opt) {
        case 'n':    flags |= O_NONBLOCK;    break;
        default:     usage(stderr, argv[0]); exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    mqd = mq_open(argv[optind], flags);
    if (mqd == (mqd_t) -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    if (mq_getattr(mqd, &attr) == -1) {
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    num_read = mq_receive(mqd, buffer, attr.mq_msgsize, &prio);
    if (num_read == -1) {
        perror("mq_receive");
        exit(EXIT_FAILURE);
    }

    printf("Read %ld bytes; priority = %u\n", (long) num_read, prio);
    if (write(STDOUT_FILENO, buffer, num_read) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
