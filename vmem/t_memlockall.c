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
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int
main(int argc, char *argv[])
{
    int retc;
    char *p;
    size_t size;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s size\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    size = strtoul(argv[1], NULL, 0);

    retc = mlockall(MCL_FUTURE);
    if (retc == -1) {
        perror("mlockall");
        exit(EXIT_FAILURE);
    }

    p = malloc(size);
    if (p == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
