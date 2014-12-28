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
    int fd, newfd;
    char tmp[PATH_MAX] = "/tmp/flock_dup";

    fd = open(tmp, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    (void) unlink(tmp);

    if (flock(fd, LOCK_EX) == -1) {
        perror("flock");
        exit(EXIT_FAILURE);
    }

    printf("BEFORE: \n");
    (void) system("cat /proc/locks");
    newfd = dup(fd);
    if (newfd == -1) {
        perror("dup");
        exit(EXIT_FAILURE);
    }
    if (flock(newfd, LOCK_EX | LOCK_NB) == -1) {
        perror("flock");
        exit(EXIT_FAILURE);
    }
    if (flock(newfd, LOCK_UN) == -1) {
        perror("flock");
        exit(EXIT_FAILURE);
    }
    printf("AFTER: \n");
    (void) system("cat /proc/locks");

    exit(EXIT_SUCCESS);
}
