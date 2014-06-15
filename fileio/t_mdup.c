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

#include "mdup.h"


static int
is_same_flags(int fd1, int fd2)
{
    int flags1, flags2;
    flags1 = fcntl(fd1, F_GETFL);
    if (flags1 == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    flags2 = fcntl(fd2, F_GETFL);
    if (flags2 == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    printf("is_same_flags: (fd1: %d, fd2 %d)\n",
           flags1, flags2);

    return (flags1 == flags2);
}


static int
is_same_offset(int fd1, int fd2)
{
    off_t offset1, offset2;
    offset1 = lseek(fd1, 0, SEEK_CUR);
    if (offset1 == -1) {
        perror("seek");
        exit(EXIT_FAILURE);
    }
    offset2 = lseek(fd2, 0, SEEK_CUR);
    if (offset2 == -1) {
        perror("seek");
        exit(EXIT_FAILURE);
    }
    printf("is_same_offset: (fd1: %lld, fd2: %lld)\n",
        (long long)offset1, (long long)offset2);

    return (offset1 == offset2);
}


int
main(int argc, char *argv[])
{
    int fd, newfd;

    /* check arguments */
    if (argc != 2 || strcmp(argv[0], "--help") == 0) {
        fprintf(stderr, "Usage: %s pathname\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* open */
    fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (write(fd, "abcdefghijklmnopqrstuvwxyz", 26) != 26) {
        fprintf(stderr, "cannot write whole buffer\n");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd, 10, SEEK_SET) == -1) {
        perror("seel");
        exit(EXIT_FAILURE);
    }

    /* duplicate */
    newfd = mdup(fd);
    if (newfd == -1) {
        perror("mdup");
        exit(EXIT_FAILURE);
    }

    /* check flags */
    if (!is_same_flags(fd, newfd)) {
        fprintf(stderr, "file flags is not same value\n");
        exit(EXIT_FAILURE);
    }
    /* check file offset */
    if (!is_same_offset(fd, newfd)) {
        fprintf(stderr, "file offset is not same value\n");
        exit(EXIT_FAILURE);
    }

    /* close */
    if (close(newfd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    /* duplicate2 */
    if (mdup2(1000, 100) != -1) {
        fprintf(stderr, "mdup2 yields invalid fd\n");
        exit(EXIT_FAILURE);
    }
    if (mdup2(fd, fd) == -1) {
        perror("mdup2");
        exit(EXIT_FAILURE);
    }

    /* duplicate2 */
    newfd = mdup2(fd, 7);
    if (newfd == -1) {
        perror("mdup2");
        exit(EXIT_FAILURE);
    }
    if (newfd != 7) {
        fprintf(stderr, "mdup2 generate wrong fd\n");
        exit(EXIT_SUCCESS);
    }

    /* write and seek */
    if (write(newfd, "abcdefghijklmnopqrstuvwxyz", 26) != 26) {
        fprintf(stderr, "cannot write whole buffer\n");
        exit(EXIT_FAILURE);
    }
    if (lseek(newfd, 26, SEEK_SET) == -1) {
        perror("seek");
        exit(EXIT_FAILURE);
    }

    /* check flags */
    if (!is_same_flags(fd, newfd)) {
        fprintf(stderr, "file flags is not same value\n");
        exit(EXIT_FAILURE);
    }
    if (!is_same_offset(fd, newfd)) {
        fprintf(stderr, "file offset is not same value\n");
        exit(EXIT_FAILURE);
    }

    /* close */
    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
    if (close(newfd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
