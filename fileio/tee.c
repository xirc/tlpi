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
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>


#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif


static void
usage(const char *format, ...)
{
    va_list args;

    fflush(stdout);

    fprintf(stderr, "Usage: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fflush(stderr);
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
    int fd;
    int opt, opt_is_append;
    char *ofile;
    int open_flags;
    mode_t file_perms;
    ssize_t num_reads;
    char buf[BUF_SIZE];


    /* check arguments */
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usage("%s [-a] FILE\n", argv[0]);
    }

    /* parse option */
    opt_is_append = 0;
    while ((opt = getopt(argc, argv, "a")) != -1) {
        switch (opt) {
            case 'a':
                opt_is_append = 1;
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }

    /* check whether FILE is given. */
    if (opt_is_append && optind >= argc) {
        fprintf(stderr, "expected FILE, but not given.\n");
    }
    ofile = argv[optind];

    /* open file */
    open_flags = O_WRONLY | O_CREAT;
    if (opt_is_append) {
        open_flags |= O_APPEND;
    }
    file_perms =S_IRUSR | S_IWUSR | S_IRGRP; /* rw-r----- */
    fd = open(ofile, open_flags, file_perms);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /* tee (copy stdin to both FILE and stdout.) */
        while ((num_reads = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
        if (write(fd, buf, num_reads) != num_reads) {
            fprintf(stderr, "couldn't write whole buffer\n");
            exit(EXIT_FAILURE);
        }
        if (write(STDOUT_FILENO, buf, num_reads) != num_reads) {
            fprintf(stderr, "couldn't write whole buffer\n");
            exit(EXIT_FAILURE);
        }
    }
    if (num_reads == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    /* close file */
    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    return 0;
}
