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


/* t_unlink.c

    Demonstrate that, when a file is unlinked, it is not actually removed from
    the file system until after any open descriptors referring to it are closed.

    Usage: t_unlink file
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#define CMD_SIZE 200
#define BUF_SIZE 1024 /*bytes*/


int
main(int argc, char *argv[])
{
    int fd, j, num_blocks;
    char shell_cmd[CMD_SIZE];
    char buf[BUF_SIZE];

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s tmpfile [num-1kB-blocks] \n", argv[0]);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    num_blocks = (argc > 2) ? atoi(argv[2]) : 100000;
    if (num_blocks <= 0) {
        fprintf(stderr, "num-1kB-blocks %s > 0\n", argv[2]);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (unlink(argv[1]) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < num_blocks; ++j) {
        if (write(fd, buf, BUF_SIZE) != BUF_SIZE) {
            fprintf(stderr, "partial/failed write");
        }
    }

    snprintf(shell_cmd, CMD_SIZE, "df -k `dirname %s`", argv[1]);
    system(shell_cmd);

    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
    printf("*************** Closed file descriptor\n");
    system(shell_cmd);

    exit(EXIT_SUCCESS);
}
