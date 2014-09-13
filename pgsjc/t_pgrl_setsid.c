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
#include <sys/wait.h>


int
main(int argc, char *argv[])
{
    pid_t pid, sid;
    int do_parent_setsid;

    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [parent-setsid]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    do_parent_setsid = (argc > 1) ? 1 : 0;

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0 && !do_parent_setsid) {
        /* Child process do setsid */
        sid = setsid();
        if (sid == -1) {
            perror("setsid cause unexpected error");
            exit(EXIT_FAILURE);
        }
    }
    if (pid > 0 && do_parent_setsid) {
        /* Parent process do setsid */
        sid = setsid();
        if (sid == -1 && errno == EPERM) {
            printf("PARENT: setsid cause expected error (errno=%d, errmsg=%s)\n",
                    errno, strerror(errno));
        } else {
            fprintf(stderr, "setsid cause unexpected success\n");
            fprintf(stderr, "SID=%ld, ERRNO=%d, ERRMSG=%s",
                    (long) sid, errno, strerror(errno));
        }
    }

    printf("%s: PID=%ld, PPID=%ld, PGID=%ld, SID=%ld\n",
            pid > 0 ? "Parent" : "Child",
            (long) getpid(), (long) getppid(), (long) getpgrp(), (long)getsid(0));

    if (pid > 0) {
        /* parent */
        if (waitpid(pid, NULL, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
