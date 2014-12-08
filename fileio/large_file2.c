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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>


static void
usage(char *format, ...)
{
    va_list args;

    fflush(stdout);

    va_start(args, format);
    fprintf(stderr, "Usage: ");
    vfprintf(stderr, format, args);
    va_end(args);
    fflush(stderr);

    exit(EXIT_FAILURE);
}


int
main(int argc, char *argv[])
{
    int fd;
    int file_flags;
    mode_t file_perms;
    off_t offset;

    /* check arguments */
    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        usage("%s pathname offset\n", argv[0]);
    }

    /* open or create file */
    file_flags = O_RDWR | O_CREAT;
    file_perms = S_IRUSR | S_IWUSR; /* rw------- */
    fd = open(argv[1], file_flags, file_perms);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /* seek and write! */
    offset = atoll(argv[2]);
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("lseek");
        exit(EXIT_FAILURE);
    }
    if (write(fd, "test", 4) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    /* close */
    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
