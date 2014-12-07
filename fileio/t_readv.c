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


/* t_readv.c

   Demonstrate the use of the readv() system call to perform "gather I/O".

   (This program is merely intended to provide a code snippet for the book;
   unless you construct a suitably formatted input file, it can't be
   usefully executed.)
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>

#define STR_SIZE 100


int
main(int argc, char *argv[])
{
    int fd;
    struct iovec iov[3];
    struct stat st;             /* First buffer */
    int x;                      /* Second buffer */
    char str[STR_SIZE];         /* Third buffer */
    ssize_t num_read, tot_required;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    tot_required = 0;

    iov[0].iov_base = &st;
    iov[0].iov_len = sizeof(struct stat);
    tot_required += iov[0].iov_len;

    iov[1].iov_base = &x;
    iov[1].iov_len = sizeof(x);
    tot_required += iov[1].iov_len;

    iov[2].iov_base = str;
    iov[2].iov_len = STR_SIZE;
    tot_required += iov[2].iov_len;

    num_read = readv(fd, iov, 3);
    if (num_read == -1) {
        perror("readv");
        exit(EXIT_FAILURE);
    }

    if (num_read < tot_required) {
        printf("Read fewer bytes than requested\n");
    }

    printf("total bytes requested: %ld; bytes read: %ld\n",
            (long) tot_required, (long) num_read);
    exit(EXIT_SUCCESS);
}
