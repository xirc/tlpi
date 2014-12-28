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


/* pmsg_create.c

   Create a POSIX message queue.

   Usage as shown in usageError().

   Linux supports POSIX message queues since kernel 2.6.6.
*/


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <limits.h>


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp, "Usage: %s [-cx] [-m maxmsg] [-s msgsize] mq-name "
            "[octal-perms]\n", progname);
    fprintf(fp, "    -c          Create queue (O_CREAT)\n");
    fprintf(fp, "    -m maxmsg   Set maximum # of messages\n");
    fprintf(fp, "    -s msgsize  Set maximum message size\n");
    fprintf(fp, "    -x          Create exclusively (O_EXCL)\n");
}


static unsigned int
to_uoctint(const char *str, const char *name)
{
    char *endptr;
    unsigned long val;

    if (str == NULL || str[0] == '\0') {
        /* Empty string */
        goto FAIL;
    }

    errno = 0;
    val = strtoul(str, &endptr, 8);
    if (errno != 0) {
        /* Some unexpected erorr */
        goto FAIL;
    }
    if (*endptr != '\0') {
        /* Further character after number */
        goto FAIL;
    }

    if (val > UINT_MAX) {
        /* Overflow */
        goto FAIL;
    }
    return val;

FAIL:
    fprintf(stderr, "Invalid number '%s':%s\n", name, str);
    exit(EXIT_FAILURE);
}


int
main(int argc, char *argv[])
{
    int flags, opt;
    mode_t perms;
    mqd_t mqd;
    struct mq_attr attr, *attrp;

    attrp = NULL;
    attr.mq_maxmsg = 50;
    attr.mq_msgsize = 2048;
    flags = O_RDWR;

    /* Parse command-line options */
    while ((opt = getopt(argc, argv, "cm:s:x")) != -1) {
        switch (opt) {
        case 'c':
            flags |= O_CREAT;
            break;

        case 'm':
            attr.mq_maxmsg = atoi(optarg);
            attrp = &attr;
            break;

        case 's':
            attr.mq_msgsize = atoi(optarg);
            attrp = &attr;
            break;

        case 'x':
            flags |= O_EXCL;
            break;

        default:
            usage(stderr, argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    perms = (argc <= optind + 1) ? (S_IRUSR | S_IWUSR) :
        to_uoctint(argv[optind + 1], "octal-perms");

    mqd = mq_open(argv[optind], flags, perms, attrp);
    if (mqd == (mqd_t) -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
