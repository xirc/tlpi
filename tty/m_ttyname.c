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
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>


static int
_ttyname(const char *dirpath, unsigned int devid, char *ttypath)
{
    DIR *dirp;
    struct dirent *ent;
    struct stat st;
    char path[PATH_MAX];
    int required_pathlen;
    int is_found;

    dirp = opendir(dirpath);
    if (dirp == NULL) {
        return -1;
    }

    is_found = 0;
    while (1) {
        errno = 0;
        ent = readdir(dirp);
        if (ent == NULL) {
            break;
        }
        required_pathlen = strlen(dirpath) + 1 + strlen(ent->d_name) + 1;
        if (required_pathlen >= PATH_MAX) {
            continue;
        }
        (void) snprintf(path, PATH_MAX, "%s/%s", dirpath, ent->d_name);

        if (stat(path, &st) == -1) {
            continue;
        }
        if (S_ISCHR(st.st_mode) && st.st_rdev == devid) {
            (void) strncpy(ttypath, path, PATH_MAX);
            is_found = 1;
            return 0;
        }
    }
    if (!is_found && errno != 0) {
        (void) closedir(dirp);
        return -1;
    }

    if (closedir(dirp) == -1) {
        return -1;
    }

    return is_found ? 0 : -1;
}


static char*
m_ttyname(int fd)
{
    static char name[PATH_MAX];
    int devid;
    struct stat st;

    if (isatty(fd) != 1) {
        return NULL;
    }
    if (fstat(fd, &st) == -1) {
        return NULL;
    }
    devid = st.st_rdev;

    if (_ttyname("/dev/pts", devid, name) == 0) {
        return name;
    }
    if (_ttyname("/dev", devid, name) == 0) {
        return name;
    }
    return NULL;
}


int
main(int argc, char *argv[])
{
    int j, fd;
    char *name;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [file]...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (j = 1; j < argc; ++j) {
        fd = open(argv[j], O_RDONLY);
        if (fd == -1) {
            fprintf(stderr, "%s: open %s\n", strerror(errno), argv[j]);
            continue;
        }
        name = ttyname(fd);
        printf("%s %s\n", argv[j], name != NULL ? name : "NOTTY");
        name = m_ttyname(fd);
        printf("%s %s\n", argv[j], name != NULL ? name : "NOTTY");
    }

    exit(EXIT_SUCCESS);
}
