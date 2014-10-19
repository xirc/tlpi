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
#include <time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "binary_sems_fifo.h"


#define FIFO_FILEPATH "/tmp/binary_sems_fifo.fifo"


static void
cleanup(void)
{
    (void) unlink(FIFO_FILEPATH);
}


static void
psemval(struct fifosem *fs __attribute__((unused)))
{
    /* Cannot examine fifo size */
    printf("FIFO length = ?\n");
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct fifosem fs;
    time_t t;

    /* Set cleanup */
    if (atexit(cleanup) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    /* Create fifo as semaphore*/
    if (mkfifo(FIFO_FILEPATH, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    /* Initialize struct fifosem */
    if (init_fifosem(FIFO_FILEPATH, &fs) == -1) {
        perror("init_fifosem");
        exit(EXIT_FAILURE);
    }

    /* Init semaphore in use */
    printf("init_sem_in_use\n");
    if (init_sem_in_use(&fs) == -1) {
        perror("init_sem_in_use");
        exit(EXIT_FAILURE);
    }
    psemval(&fs);

    /* Init semaphore available */
    printf("init_sem_available\n");
    if (init_sem_available(&fs) == -1) {
        perror("init_sem_available");
        exit(EXIT_FAILURE);
    }
    psemval(&fs);

    /* Reserve semaphore */
    printf("reserve_sem\n");
    if (reserve_sem(&fs) == -1) {
        perror("reserve_sem");
        exit(EXIT_FAILURE);
    }
    psemval(&fs);

    /* Reserve semaphore conditionally */
    printf("reserve_sem_nb expects FAIL\n");
    if (reserve_sem_nb(&fs) == -1) {
        perror("reserve_sem_nb");
        /* Do not exit */
    }
    psemval(&fs);

    printf("release_sem\n");
    if (release_sem(&fs) == -1) {
        perror("release_sem");
        exit(EXIT_FAILURE);
    }
    psemval(&fs);

    printf("reserve_sem_nb\n");
    if (reserve_sem_nb(&fs) == -1) {
        perror("reserve_sem_nb");
        exit(EXIT_FAILURE);
    }
    psemval(&fs);

    printf("SYNC using binary semaphore\n");
    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:     /* Child */
        sleep(3);
        if (release_sem(&fs) == -1) {
            perror("reserve_sem");
            exit(EXIT_FAILURE);
        }
        t = time(NULL);
        printf("CHILD: time=%s", ctime(&t));
        exit(EXIT_SUCCESS);
    default:    /* Parent */
        sleep(1);
        if (reserve_sem(&fs) == -1) {
            perror("reserve_sem");
            exit(EXIT_FAILURE);
        }
        t = time(NULL);
        printf("PARENT: time=%s", ctime(&t));
        /* fall through */
    }
    if (wait(NULL) == -1) {
        perror("wait");
    }

    exit(EXIT_SUCCESS);
}
