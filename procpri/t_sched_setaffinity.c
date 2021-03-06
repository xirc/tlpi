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
    int cpu;
    unsigned long mask;

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s pid mask\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid = atoi(argv[1]);
    if (pid < 0) {
        fprintf(stderr, "pid %s >= 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    mask = atol(argv[2]);
    if (mask <= 0) {
        fprintf(stderr, "mask %s > 0\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    CPU_ZERO(&set);
    for (cpu = 0; mask > 0; cpu++, mask >>= 1) {
        if (mask & 1) {
            CPU_SET(cpu, &set);
        }
    }

    if (sched_setaffinity(pid, sizeof(set), &set) == -1) {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
