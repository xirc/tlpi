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


/* t_select.c

   Example of the use of the select() system call to monitor multiple
   file descriptors.

   Usage as shown in usage().
*/


#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static void
usage(FILE *fp, const char* progname)
{
    fprintf(fp, "Usage: %s {timeout|-} fd-num[rw]...\n", progname);
    fprintf(fp, "    - means infinite timeout; \n");
    fprintf(fp, "    r = monitor for read\n");
    fprintf(fp, "    w = monitor for write\n\n");
    fprintf(fp, "    e.g.: %s - 0rw 1w\n", progname);
}


int
main(int argc, char *argv[])
{
    fd_set readfds, writefds;
    int ready, nfds, fd, num_read, j;
    struct timeval timeout;
    struct timeval *pto;
    char buf[10];
        /* Large enough to hold "rw0" */

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Timeout for select() is specified in argv[1] */
    if (strcmp(argv[1], "-") == 0) {
        pto = NULL;     /* Infinite timeout */
    } else {
        pto = &timeout;
        timeout.tv_sec = strtoul(argv[1], NULL, 0);
        timeout.tv_usec = 0;
    }

    /* Process remaining arguments to build file descriptor sets */
    nfds = 0;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    for (j = 2; j < argc; ++j) {
        num_read = sscanf(argv[j], "%d%2[rw]", &fd, buf);
        if (num_read != 2) {
            usage(stderr, argv[0]);
            exit(EXIT_FAILURE);
        }
        if (fd >= FD_SETSIZE) {
            fprintf(stderr, "file descriptor exceeds limit (%d)\n", FD_SETSIZE);
            usage(stderr, argv[0]);
            exit(EXIT_FAILURE);
        }

        if (fd >= nfds) {
            nfds = fd + 1;
        }
        if (strchr(buf, 'r') != NULL) {
            FD_SET(fd, &readfds);
        }
        if (strchr(buf, 'w') != NULL) {
            FD_SET(fd, &writefds);
        }
    }

    /* We've built all of the arguments; now call select() */
    ready = select(nfds, &readfds, &writefds, NULL, pto);
        /* Ignore exceptional events */

    if (ready == -1) {
        perror("select");
        exit(EXIT_FAILURE);
    }

    /* Display result of select() */
    printf("ready = %d\n", ready);
    for (fd = 0; fd < nfds; ++fd) {
        printf("%d: %s%s\n", fd,
                FD_ISSET(fd, &readfds) ? "r" : "",
                FD_ISSET(fd, &writefds) ? "w" : "");
    }

    if (pto != NULL) {
        printf("timeout after select(): %ld.%03ld\n",
                (long) timeout.tv_sec, (long) timeout.tv_usec / 10000);
    }

    exit(EXIT_SUCCESS);
}
