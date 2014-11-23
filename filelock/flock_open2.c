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
#include <sys/file.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int fd1, fd2;
    char tmp[PATH_MAX] = "/tmp/flock_open2";

    fd1 = open(tmp, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd1 == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    fd2 = open(tmp, O_RDWR);
    if (fd2 == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    (void) unlink(tmp);

    if (flock(fd1, LOCK_EX | LOCK_NB) == -1) {
        perror("flock 1");
        exit(EXIT_FAILURE);
    }
    if (flock(fd2, LOCK_EX | LOCK_NB) == -1) {
        perror("flock 2");
        exit(EXIT_FAILURE);
    }
    (void) system("cat /proc/locks");

    exit(EXIT_SUCCESS);
}
