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
#include <sys/xattr.h>


static void
usage(const char *progname)
{
    fprintf(stderr, "usage: %s -n name [-v value]\n files...\n", progname);
    fflush(stderr);
}


int
main(int argc, char *argv[])
{
    int i, opt, failed;
    char *name, *value;

    name = NULL;
    value = "";
    while ((opt = getopt(argc, argv, "n:v:")) != -1) {
        switch (opt) {
        case 'n':
            name = optarg;
            break;
        case 'v':
            value = optarg;
            break;
        default:
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (name == NULL) {
        fprintf(stderr, "'-n name' must be specified.\n");
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    failed = 0;
    for (i = optind; i < argc; ++i) {
        if (setxattr(argv[i], name, value, strlen(value), 0) == -1) {
            fprintf(stderr, "%s %s\n", strerror(errno), argv[i]);
            failed = 1;
        }
    }
    if (failed) {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
