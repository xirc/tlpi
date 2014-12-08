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
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef BUF_SIZE        /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024
#endif


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp, "Usage: %s old-file new-file\n", progname);
}


int
main(int argc, char *argv[])
{
    int ifd, ofd, oflags;
    mode_t perms;
    ssize_t num_read;
    char buf[BUF_SIZE];

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (argc != 3) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Open input and output files */
    ifd = open(argv[1], O_RDONLY);
    if (ifd == -1) {
        fprintf(stderr, "open file %s: %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    oflags = O_CREAT | O_WRONLY | O_TRUNC;
    perms = /* rw-rw-rw- */
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    ofd = open(argv[2], oflags, perms);
    if (ofd == -1) {
        fprintf(stderr, "open file %s: %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Transfer data until we encounter end of input or an error */
    while ((num_read = read(ifd, buf, BUF_SIZE)) > 0) {
        if (write(ofd, buf, num_read) != num_read) {
            fprintf(stderr, "couldn't write whole buffer\n");
            exit(EXIT_FAILURE);
        }
    }
    if (num_read == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    if (close(ifd) == -1) {
        perror("close - input");
        exit(EXIT_FAILURE);
    }
    if (close(ofd) == -1) {
        perror("close - output");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
