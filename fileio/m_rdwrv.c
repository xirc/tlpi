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
#include <sys/uio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>

#define STR_SIZE 100
#define X (0xfee1dead)
#define STR "FEELDEAD"


static ssize_t m_readv(int fd, const struct iovec *iov, int iovcnt);
static ssize_t m_writev(int fd, const struct iovec *iov, int iovcnt);


static void
usage(char *format, ...)
{
    va_list args;

    fflush(stdout);

    fprintf(stderr, "Usage: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    exit(EXIT_FAILURE);
}


int
main(int argc, char *argv[])
{
    int fd;
    struct iovec iov[3];
    struct stat my_struct;
    unsigned int x;
    char str[STR_SIZE];
    ssize_t num_reads, num_write, total_required;

    /* check arguments */
    if (argc != 2 || strcmp(argv[1], "--help") == 0){
        usage("%s file\n", argv[0]);
    }

    /* open file */
    fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /* initialize data */
    if (stat("/dev/sda", &my_struct)) {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    x = X;
    strcpy(str, STR);

    /* write */
    total_required = 0;

    iov[0].iov_base = &my_struct;
    iov[0].iov_len = sizeof(struct stat);
    total_required += iov[0].iov_len;

    iov[1].iov_base = &x;
    iov[1].iov_len = sizeof(x);
    total_required += iov[1].iov_len;

    iov[2].iov_base = str;
    iov[2].iov_len = STR_SIZE;
    total_required += iov[2].iov_len;

    num_write = m_writev(fd, iov, 3);
    if (num_write == -1) {
        perror("writev");
        exit(EXIT_FAILURE);
    }
    if (num_write < total_required) {
        fprintf(stderr, "write fewer bytes than requested\n");
    }
    printf("total bytes requested : %ld; bytes write: %ld\n",
            (long) total_required, (long) num_write);

    /* initialize */
    memset(&my_struct, 0, sizeof(struct stat));
    x = 0;
    memset(str, 0, STR_SIZE);

    /* read */
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("seek");
        exit(EXIT_FAILURE);
    }
    num_reads = m_readv(fd, iov, 3);
    if (num_reads == -1) {
        perror("readv");
        exit(EXIT_FAILURE);
    }
    if (num_reads < total_required) {
        fprintf(stderr, "Read fewer bytes than requested\n");
    }
    printf("total bytes requested : %ld; bytes read: %ld\n",
            (long) total_required, (long) num_reads);

    /* check data */
    if (x == X) {
        printf("x is successfully restored.\n");
    } else {
        printf("x is NOT successfully restored.\n");
    }
    if (strcmp(str, STR) == 0) {
        printf("str is successfully restored.\n");
    } else {
        printf("str is NOT successfully restored.\n");
    }

    exit(EXIT_SUCCESS);
}


static ssize_t
m_readv(int fd, const struct iovec *iov, int iovcnt)
{
    int i;
    ssize_t total_required, num_reads, begin;
    char *buffer;

    total_required = 0;
    for (i = 0; i < iovcnt; ++i) {
        total_required += iov[i].iov_len;
    }

    buffer = malloc(total_required);
    if (buffer == NULL) {
        errno = ENOMEM;
        return -1;
    }

    num_reads = read(fd, buffer, total_required);
    if (num_reads == -1) {
        free(buffer);
        return -1;
    }

    begin = 0;
    for (i = 0; i < iovcnt; ++i) {
        memcpy(iov[i].iov_base, &buffer[begin], iov[i].iov_len);
        begin += iov[i].iov_len;
    }

    free(buffer);
    return num_reads;
}


static ssize_t
m_writev(int fd, const struct iovec *iov, int iovcnt)
{
    int i;
    ssize_t total_required, num_write, begin;
    char *buffer;

    total_required = 0;
    for (i = 0; i < iovcnt; ++i) {
        total_required += iov[i].iov_len;
    }

    buffer = malloc(total_required);
    if (buffer == NULL) {
        errno = ENOMEM;
        return -1;
    }

    begin = 0;
    for (i = 0; i < iovcnt; ++i) {
        memcpy(&buffer[begin], iov[i].iov_base, iov[i].iov_len);
        begin += iov[i].iov_len;
    }

    num_write = write(fd, buffer, total_required);
    free(buffer);
    return num_write;
}
