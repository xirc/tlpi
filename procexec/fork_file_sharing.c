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


/* fork_file_sharing.c

   Show that the file descriptors of a forked child refer to the
   same open file objects as the parent.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int fd, flags;
    char template[] = "/tmp/testXXXXXX";

    setbuf(stdout, NULL);
        /* Disable buffering of stdout */

    fd = mkstemp(template);
    if (fd == -1) {
        perror("mkstemp");
        exit(EXIT_FAILURE);
    }

    printf("File offset before fork(): %lld\n",
            (long long) lseek(fd, 0, SEEK_CUR));

    flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        perror("fcntl - F_GETFL");
        exit(EXIT_FAILURE);
    }
    printf("O_APPEND flag before fork() is: %s\n",
            (flags & O_APPEND) ? "on" : "off");

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        /* Child : change file offset and status flags */
        if (lseek(fd, 1000, SEEK_SET) == -1) {
            perror("lseek");
            exit(EXIT_FAILURE);
        }

        flags = fcntl(fd, F_GETFL);
            /* Fetch current flags */
        if (flags == -1) {
            perror("fcntl - F_GETFL");
            exit(EXIT_FAILURE);
        }
        flags |= O_APPEND;
        if (fcntl(fd, F_SETFL, flags) == -1) {
            perror("fcntl - F_SETFL");
            exit(EXIT_FAILURE);
        }
        _exit(EXIT_SUCCESS);
    default:
        /* Parent: can see file changes made by child */
        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
        printf("Child has exited");

        printf("File offset in parent: %lld\n",
                (long long) lseek(fd, 0, SEEK_CUR));

        flags = fcntl(fd, F_GETFL);
        if (flags == -1) {
            perror("fcntl - F_GETFL");
            exit(EXIT_FAILURE);
        }
        printf("O_APPEND flag in parent is: %s\n",
                (flags & O_APPEND) ? "on" : "off");
        exit(EXIT_SUCCESS);
    }
}
