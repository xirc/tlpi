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
#include <sys/stat.h>
#include <string.h>


static int
chmod_aprX(const char *filepath)
/* chmod a+rX */
{
    struct stat s;
    mode_t new_mode;

    if (stat(filepath, &s) == -1) {
        return -1;
    }

    new_mode = s.st_mode;
    new_mode |= S_IRUSR | S_IRGRP | S_IROTH;

    switch (s.st_mode & S_IFMT) {
    case S_IFREG:
        if ((new_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) == 0) {
            break;
        }
        /* fall through */
    case S_IFDIR:
        new_mode |= S_IXUSR | S_IXGRP | S_IXOTH;
        break;
    default:
        return -1;
    }

    if (chmod(filepath, new_mode) == -1) {
        return -1;
    }

    return 0;
}


int
main(int argc, char *argv[])
{
    int i, fail;
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s filepath...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fail = 0;
    for (i = 1; i < argc; ++i) {
        if (chmod_aprX(argv[i]) == -1) {
            fprintf(stderr, "'chmod a+rX %s' failed\n", argv[i]);
            fail = 1;
        }
    }
    if (fail) {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}


