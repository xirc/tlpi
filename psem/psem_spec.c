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
#include <semaphore.h>
#include <fcntl.h>


static void
usage_exit(char *progname, int exit_code)
{
    fprintf(stderr, "Usage: %s sem-name COUNT\n", progname);
    exit(exit_code);
}


int
main(int argc, char *argv[])
{
    char *opt_semname;
    unsigned opt_count;
    sem_t *sem;
    unsigned long i;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        usage_exit(argv[0], EXIT_SUCCESS);
    }
    if (argc < 3) {
        usage_exit(argv[0], EXIT_FAILURE);
    }

    opt_semname = argv[1];
    opt_count = strtoul(argv[2], NULL, 0);

    sem = sem_open(opt_semname, O_CREAT | O_EXCL);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < opt_count; ++i) {
        if (sem_wait(sem) == -1) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }
        if (sem_post(sem) == -1) {
            perror("sem_post");
            exit(EXIT_FAILURE);
        }
    }

    if (sem_close(sem) == -1) {
        perror("sem_close");
        exit(EXIT_FAILURE);
    }
    if (sem_unlink(opt_semname) == -1) {
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
