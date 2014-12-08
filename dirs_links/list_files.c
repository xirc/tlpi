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


/* list_files.c

   Demonstrate the use of opendir() and related functions to list files
   in a directory.

   Walk through each directory named on the command line (current directory
   if none are specified) to display a list of the files it contains.

    Usage: list_files [dir...]
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>


static void
list_files(const char *dirpath)
{
    DIR *dirp;
    struct dirent *dp;
    int is_current;

    is_current = strcmp(dirpath, ".") == 0;

    dirp = opendir(dirpath);
    if (dirp == NULL) {
        fprintf(stderr, "open dir failed on '%s'", dirpath);
        return;
    }

    while (1) {
        errno = 0;
        dp = readdir(dirp);
        if (dp == NULL) {
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
    if (errno != 0) {
        perror("readdir");
        exit(EXIT_FAILURE);
    }
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
