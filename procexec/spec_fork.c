/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _BSD_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <stdint.h>


static void
usage(FILE *fp, char const *prog_name)
{
        fprintf(fp, "usage: %s -nvs\n", prog_name);
        fprintf(fp, "    n: number of forks (int)\n");
        fprintf(fp, "    v: use vfork\n");
        fprintf(fp, "    s: process size [MB] (int)\n");
}


int
main(int argc, char *argv[])
{
    int opt;
    int nforks, use_vfork, proc_size;
    int i, status;
    pid_t pid;

    static uint8_t *buffer;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    nforks = 1000;
    use_vfork = 0;
    proc_size = 0;
    while ((opt = getopt(argc, argv, "n:vs:")) != -1) {
        switch (opt) {
        case 'n':
            nforks = atoi(optarg);
            if (nforks < 0) {
                fprintf(stderr, "(number of forks) %s >= 0\n", optarg);
                exit(EXIT_FAILURE);
            }
            break;
        case 'v':
            use_vfork = 1;
            break;
        case 's':
            proc_size = atoi(optarg);
            if (proc_size < 0) {
                fprintf(stderr, "(proccess size) %s >= 0\n", optarg);
                exit(EXIT_FAILURE);
            }
            break;
        default:
            usage(stderr, argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    buffer = NULL;
    if (proc_size > 0) {
        buffer = malloc(proc_size * sizeof(uint8_t) * 1024 * 1024);
        if (buffer == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < nforks; ++i) {
        pid = use_vfork ? vfork() : fork();
        switch (pid) {
        case -1:
            perror(use_vfork ? "vfork" : "fork");
            exit(EXIT_FAILURE);
        case 0:
            _exit(EXIT_SUCCESS);
        default:
            if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (buffer != NULL) {
        free(buffer);
    }
    exit(EXIT_SUCCESS);
}
