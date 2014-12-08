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
    int fdi, fdo;
    ssize_t num_reads;
    char buffer[BUF_SIZE];
    int file_flags;
    mode_t file_perms;
    ssize_t offset, i;
    char zero[1] = {0};

    /* check arguments*/
    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        usage("%s src dest\n", argv[0]);
    }

    /* open input file */
    fdi = open(argv[1], O_RDONLY);
    if (fdi == -1) {
        perror("open input file");
        exit(EXIT_FAILURE);
    }

    /* open output file */
    file_flags = O_WRONLY | O_CREAT | O_TRUNC;
    file_perms = S_IRUSR | S_IWUSR | S_IRGRP; /* rw-r----- */
    fdo = open(argv[2], file_flags, file_perms);
    if (fdo == -1) {
        perror("open output file");
        exit(EXIT_FAILURE);
    }

    /* copy */
    while ((num_reads = read(fdi, buffer, BUF_SIZE)) > 0) {
        offset = 0;
        while (offset < num_reads) {
            if (buffer[offset] != 0) {
                // search file hole
                for (i = offset + 1; i + 1 < num_reads; ++i) {
                    if (buffer[i] == 0 && buffer[i+1] == 0) {
                        break;
                    }
                }
                // write
                if (write(fdo, &buffer[offset], (i - offset)) != (i -offset)) {
                    fprintf(stderr, "couldn't write whole buffer");
                    exit(EXIT_FAILURE);
                }
            } else {
                // search data
                for (i = offset + 1; i < num_reads; ++i) {
                    if (buffer[i] != 0) {
                        break;
                    }
                }
                // seek or write
                if (i - offset > 1) {
                    // seek
                    if (lseek(fdo, (i - offset), SEEK_CUR)  == -1) {
                        perror("seek");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    // write 0
                    if (write(fdo, zero, 1) != 1) {
                        perror("write null");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            offset = i;
        }
    }
    if (num_reads == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    /* close */
    if (close(fdi) == -1) {
        perror("close input file");
        exit(EXIT_FAILURE);
    }
    if (close(fdo) == -1) {
        perror("close output file");
        exit(EXIT_FAILURE);
    }

    return 0;
}
