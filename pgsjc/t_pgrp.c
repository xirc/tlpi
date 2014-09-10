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
#include <sys/wait.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    pid_t pipeline_pgid;
    pid_t pid;

    /* disable buffering of stdout */
    setbuf(stdout, NULL);

    pipeline_pgid = getppid();
    printf("About to set pipeline_pgid = %ld\n", (long) pipeline_pgid);

    pid = fork();
    switch (pid) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0: /* child */
        printf("CHILD:  before pid=%ld pgid=%ld\n",
                (long) getpid(), (long) getpgrp());
        if (setpgid(0, pipeline_pgid) == -1) {
            perror("setpgid");
        }
        printf("CHILD:  after  pid=%ld pgid=%ld\n",
                (long) getpid(), (long) getpgrp());
        _exit(EXIT_SUCCESS);
    default: /* parent */
        printf("PARENT: before pid=%ld pgid=%ld\n",
                (long) getpid(), (long) getpgrp());
        if (setpgid(0, pipeline_pgid) == -1 && errno != EACCES) {
            perror("setpgid");
        }
        printf("PARENT: after  pid=%ld pgid=%ld\n",
                (long) getpid(), (long) getpgrp());
    }

    if (wait(NULL) == -1) {
        perror("wait");
    }
    exit(EXIT_SUCCESS);
}
