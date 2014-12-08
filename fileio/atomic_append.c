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
#include <stdarg.h>
#include <fcntl.h>


/*
 * O_APPEND を使用した場合
 * ./atomic_append f1 100000 & ./atomic_append f1 100000
 * f1のファイルサイズは必ず200000となる。
 *
 * O_APPEND_を使用せず、SEEKした場合
 * ./atomic_append f2 100000 & ./atomic_append f2 100000
 * f2のファイルサイズは200000とはならないかもしれない。
 * 100000 ~ 200000 になる。
 *
 */


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
    int fd, file_flags;
    mode_t file_perms;
    int use_lseek, flag, num_bytes;
    int i;

    /* check arguments */
    if (argc < 3 || strcmp(argv[0], "--help") == 0) {
        usage("%s filename num-bytes [x]\n", argv[0]);
    }
    use_lseek = (argc == 4);
    flag = use_lseek ? 0 : O_APPEND;
    num_bytes = atoi(argv[2]);
    if (num_bytes <= 0) {
        fprintf(stderr, "num-bytes: expect positive, but got negative\n");
        exit(EXIT_FAILURE);
    }

    /* open */
    file_flags = O_WRONLY | O_CREAT | flag;
    file_perms = S_IRUSR | S_IWUSR; /* rw------- */
    fd = open(argv[1], file_flags, file_perms);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /* write */
    for (i = 0; i < num_bytes; ++i) {
        if (use_lseek) {
            if (lseek(fd, 0, SEEK_END) == -1) {
                perror("seek");
                exit(EXIT_FAILURE);
            }
        }
        if (write(fd, "x", 1) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }
    printf("[%d] write (%d)\n", getpid(), num_bytes);

    /* close */
    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
