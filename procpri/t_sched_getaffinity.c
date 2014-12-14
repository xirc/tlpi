/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>


int
main(int argc, char *argv[])
{
    pid_t pid;
    cpu_set_t set;
    int cpu, s;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s pid\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid = atoi(argv[1]);
    if (pid < 0) {
        fprintf(stderr, "pid %s >= 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    s = sched_getaffinity(pid, sizeof(cpu_set_t), &set);
    if (s == -1) {
        perror("sched_getaffinity");
        exit(EXIT_FAILURE);
    }

    printf("CPUs:");
    for (cpu = 0; cpu < CPU_SETSIZE; cpu++) {
        if (CPU_ISSET(cpu, &set)) {
            printf(" %d", cpu);
        }
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}
