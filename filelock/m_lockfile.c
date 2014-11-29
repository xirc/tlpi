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
#include <fcntl.h>
#include <getopt.h>


int
main(int argc, char *argv[])
{
    int opt;
    int opt_sleep_time, opt_max_retries;
    int argi, fdi;
    int i, retries;
    char *filename;
    int fd;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s [-sr] filename ...\n", argv[0]);
        fprintf(stderr, "          -s SLEEP_TIME_SEC (default=1)\n");
        fprintf(stderr, "          -r MAX_RETRIES (default=unlimited)\n");
        exit(EXIT_FAILURE);
    }

    opt_sleep_time = 1;
    opt_max_retries = -1;
    while ((opt = getopt(argc, argv, "s:r:")) != -1) {
        switch (opt) {
        case 's':
            opt_sleep_time = strtoul(optarg, NULL, 0);
            break;
        case 'r':
            opt_max_retries = strtol(optarg, NULL, 0);
            break;
        }
    }

    argi = optind;
    fdi = 0;
    retries = 0;
    while (argi < argc) {
        filename = argv[argi];
        fd = open(filename, O_RDONLY | O_CREAT | O_EXCL,
                S_IRUSR | S_IRGRP);
        if (fd > 0) {
            /* SUCCESS */
            ++argi;
            ++fdi;
            continue;
        }
        /* FAILED */
        retries++;
        if (opt_max_retries >= 0 && retries > opt_max_retries) {
            goto FAIL;
        }
        (void) sleep(opt_sleep_time);
    }

    exit(EXIT_SUCCESS);

FAIL:
    for (i = optind; i < argi; ++i) {
        (void) unlink(argv[i]);
    }
    exit(EXIT_FAILURE);
}
