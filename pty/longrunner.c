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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp, "usage: %s ms [msg...]\n", progname);
}


int
main(int argc, char *argv[])
{
    int j;
    int sleep_ms;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (argc < 2) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }
    sleep_ms = strtoul(argv[1], NULL, 0);

    while (1) {
        for (j = 2; j < argc; ++j) {
            printf("%s\n", argv[j]);
        }
        (void) usleep(sleep_ms * 1000);
    }
}
