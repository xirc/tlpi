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
#include <sys/resource.h>
#include <string.h>
#include <sys/wait.h>

#include "print_rusage.h"


int
main(int argc, char *argv[])
{
    int retc;
    pid_t cpid;
    int status;
    int who;
    struct rusage res_usage;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s command [arg]...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    switch (cpid = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        /* child */
        retc = execvp(argv[1], argv+1);
        /* If we come here, execvp was failed */
        perror("exec");
        exit(EXIT_FAILURE);
    default:
        /* parent */
        retc = waitpid(cpid, &status, 0);
        if (retc == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        break;
    }

    who = RUSAGE_CHILDREN;
    retc = getrusage(who, &res_usage);
    if (retc == -1) {
        perror("getrusage");
        exit(EXIT_FAILURE);
    }
    print_rusage("  ", &res_usage);

    exit(WEXITSTATUS(status));
}
