/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


/*
 * EXERCISE 5-2
 * O_APPEND を用いて開いてファイルを開き、
 * ファイルの先頭へ移動し、データを書き込む。
 *
 * O_APPEND フラグを用いた場合、
 * write時に、
 * 　ファイルの末尾へ移動
 * 　データの書き込み
 * がアトミックに行われる。
 * 従って、SEEKによってファイルの先頭へ移動しても、
 * 常にファイルの末尾へデータが書き込まれる。
 *
 */


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


static void
usage(char *format, ...)
{
    va_list args;

    fflush(stdout);

    va_start(args, format);
    fprintf(stderr, "Usage: ");
    vfprintf(stderr, format, args);
    va_end(args);

    exit(EXIT_FAILURE);
}


int
main(int argc, char *argv[])
{
    int fd, file_flags;
    mode_t file_perms;

    /* Check arguments */
    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usage("%s pathname write_data\n", argv[0]);
    }

    /* Open file */
    file_flags = O_RDWR | O_APPEND;
    file_perms = S_IRUSR | S_IWUSR; /* rw------- */
    fd = open(argv[1], file_flags, file_perms);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /* seek and write! */
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("seek");
        exit(EXIT_FAILURE);
    }
    if (write(fd, argv[2], strlen(argv[2])) == -1) {
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
