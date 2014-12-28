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
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <time.h>


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp, "Usage: %s [-nt] name\n", progname);
    fprintf(fp, "    -n               Use O_NONBLOCK flag\n");
    fprintf(fp, "    -t  TIMEOUT_SEC  Use timedreceive\n");
}


int
main(int argc, char *argv[])
{
    int flags, opt, opt_n, opt_t;
    mqd_t mqd;
    unsigned int prio;
    void *buffer;
    struct mq_attr attr;
    ssize_t num_read;
    time_t timeout_sec;
    struct timespec timeout;

    opt_n = 0;
    opt_t = 0;
    flags = O_RDONLY;
    timeout_sec = 0;
    while ((opt = getopt(argc, argv, "nt:")) != -1) {
        switch (opt) {
        case 'n':
            opt_n = 1;
            flags |= O_NONBLOCK;
            break;
        case 't':
            opt_t = 1;
            timeout_sec = strtoul(optarg, NULL, 0);
            break;
        default:
            usage(stderr, argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (opt_n && opt_t) {
        fprintf(stderr, "Cannot specify both -n and -t\n");
        exit(EXIT_FAILURE);
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

    if (opt_t) {
        if (clock_gettime(CLOCK_REALTIME, &timeout) == -1) {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }
        timeout.tv_sec += timeout_sec;
        num_read = mq_timedreceive(mqd, buffer,
                attr.mq_msgsize, &prio, &timeout);
    } else {
        num_read = mq_receive(mqd, buffer, attr.mq_msgsize, &prio);
    }
    if (num_read == -1) {
        perror(opt_t ? "mq_timedreceive" : "mq_receive");
        exit(EXIT_FAILURE);
    }

    printf("Read %ld bytes; priority = %u\n", (long) num_read, prio);
    if (write(STDOUT_FILENO, buffer, num_read) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
