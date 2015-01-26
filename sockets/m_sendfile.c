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
#include <fcntl.h>


static ssize_t
m_sendfile(int out_fd, int in_fd, off_t *offset, size_t count)
{
#define BUF_SIZE 8096
    char buf[BUF_SIZE];
    size_t size;
    ssize_t num_read, num_wrote;
    size_t num_sent;

    if (offset != NULL) {
        if (lseek(in_fd, *offset, SEEK_SET) == -1) {
            return -1;
        }
    }

    num_sent = 0;
    while (num_sent < count) {
        size = count < BUF_SIZE ? count : BUF_SIZE;
        num_read = read(in_fd, buf, size);
        if (num_read == -1) {
            return -1;
        }
        if (num_read == 0) {
            break;
        }
        num_wrote = write(out_fd, buf, num_read);
        if (num_wrote == -1) {
            return -1;
        }
        num_sent += num_wrote;
    }

    if (offset != NULL) {
        offset += num_sent;
    }

    return 0;
}


int
main(int argc, char *argv[])
{
    int infd, outfd;
    off_t offset, *p_offset;
    size_t count;
    ssize_t num_sent;

    if (argc < 4) {
        fprintf(stderr, "usage: %s infile outfile count [offset]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    infd = open(argv[1], O_RDONLY);
    if (infd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    outfd = open(argv[2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (outfd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    count = strtoul(argv[3], NULL, 0);

    p_offset = NULL;
    if (argc == 5) {
        offset = strtoul(argv[4], NULL, 0);
        p_offset = &offset;
    }

    num_sent = m_sendfile(outfd, infd, p_offset, count);
    if (num_sent == -1) {
        perror("m_sendfile");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
