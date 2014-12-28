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
#include <signal.h>


static void
handler(int sig __attribute__((unused)))
{
    /* do nothing */
}


int
main(int argc, char *argv[])
{
    int i;
    int fd;
    struct flock fl;
    struct sigaction sa;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    (void) sigaction(SIGINT, &sa, NULL);
    (void) sigaction(SIGTERM, &sa, NULL);

    fd = open(argv[1], O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, 80000) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0; /* It will be changed */
    fl.l_len = 1;
    for (i = 0; i < 80000; i += 2) {
        if (i % 8000 == 0) {
            printf(".");
            fflush(stdout);
        }
        fl.l_start = i;
        if (fcntl(fd, F_SETLK, &fl) == -1) {
            perror("fcntl");
            exit(EXIT_FAILURE);
        }
    }
    printf("\nLOCKED: 0 2 4 6 8 ... 79998\n");
    (void) pause();

    if (unlink(argv[1]) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
