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


/* pmsg_send.c

   Usage as shown in usageError().

   Send a message (specified as a command line argument) to a
   POSIX message queue.

   See also pmsg_receive.c.

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
    fprintf(fp, "Usage: %s [-n] name msg [prio]\n", progname);
    fprintf(fp, "    -n           Use O_NONBLOCK flag\n");
}


int
main(int argc, char *argv[])
{
    int flags, opt;
    mqd_t mqd;
    char *msg;
    unsigned int prio;

    flags = O_WRONLY;
    while ((opt = getopt(argc, argv, "n")) != -1) {
        switch (opt) {
        case 'n':   flags |= O_NONBLOCK; break;
        default:    usage(stderr, argv[0]);      exit(EXIT_FAILURE);
        }
    }

    if (optind + 1 > argc) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    mqd = mq_open(argv[optind], flags);
    if (mqd == (mqd_t) -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    msg = argv[optind + 1];
    prio = (argc > optind + 2) ? atoi(argv[optind + 2]) : 0;

    if (mq_send(mqd, msg, strlen(msg), prio) == -1) {
        perror("mq_send");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
