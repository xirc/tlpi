/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2014.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/
/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
* See above.                                                              *
\*************************************************************************/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stddef.h>


static void
list_files(const char *dirpath)
{
    DIR *dirp;
    struct dirent *dp, *result;
    int is_current;
    size_t len;

    is_current = strcmp(dirpath, ".") == 0;

    dirp = opendir(dirpath);
    if (dirp == NULL) {
        fprintf(stderr, "open dir failed on '%s'", dirpath);
        return;
    }

    len = offsetof(struct dirent, d_name) +
            pathconf(dirpath, _PC_NAME_MAX) + 1;
    dp = malloc(len);
    if (dp == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    while (1) {
        errno = readdir_r(dirp, dp, &result);
        if (errno > 0) {
            perror("readdir_r");
            exit(EXIT_FAILURE);
        }
        if (result == NULL) {
            /* no more entries */
            break;
        }

        if (strcmp(dp->d_name, ".") == 0 ||
            strcmp(dp->d_name, "..") == 0)
        {
            continue;
        }

        if (!is_current) {
            printf("%s/", dirpath);
        }
        printf("%s\n", dp->d_name);
    }
    free(dp);
    if (closedir(dirp) == -1) {
        perror("closedir");
        exit(EXIT_FAILURE);
    }
}


int
main(int argc, char *argv[])
{
    int i;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s [dir...]\n", argv[0]);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    if (argc == 1) {
        /* no arguments - use current working directory */
        list_files(".");
    } else {
        for (i = 1; i < argc; ++i) {
            list_files(argv[i]);
        }
    }

    exit(EXIT_SUCCESS);
}
