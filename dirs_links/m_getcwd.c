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
#include <fcntl.h>
#include <limits.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>


static char * m_getcwd(char *cwdbuf, size_t size);


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    char cwd[PATH_MAX];

    if (m_getcwd(cwd, PATH_MAX) == NULL) {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", cwd);

    exit(EXIT_SUCCESS);
}


static char *
m_getcwd_iter(char *cwdbuf, size_t size, int level)
{
    DIR *current, *parent;
    int current_fd, parent_fd, target_fd;
    struct dirent *dp;
    struct stat cs, ps, ts;
    char *retval;

    current = parent = NULL;
    current = opendir(".");
    if (current == NULL) {
        retval = NULL;
        goto EXIT;
    }
    parent = opendir("..");
    if (parent == NULL) {
        retval = NULL;
        goto EXIT;
    }
    current_fd = dirfd(current);
    if (current_fd == -1) {
        retval = NULL;
        goto EXIT;
    }
    parent_fd = dirfd(parent);
    if (parent_fd == -1) {
        retval = NULL;
        goto EXIT;
    }
    if (fstat(current_fd, &cs) == -1) {
        retval = NULL;
        goto EXIT;
    }
    if (fstat(parent_fd, &ps) == -1) {
        retval = NULL;
        goto EXIT;
    }

    if (cs.st_ino == ps.st_ino) {
        strcpy(cwdbuf, "/");
        retval = cwdbuf;
        goto EXIT;
    } else {
        if (fchdir(parent_fd) == -1) {
            retval = NULL;
            goto EXIT;
        }
        retval = m_getcwd_iter(cwdbuf, size, level+1);
        if (retval == NULL) {
            goto EXIT;
        }
        if (fchdir(current_fd) == -1) {
            retval = NULL;
            goto EXIT;
        }
    }

    while (1) {
        errno = 0;
        dp = readdir(parent);
        if (dp == NULL) {
            break;
        }
        target_fd = openat(parent_fd, dp->d_name, O_RDONLY);
        if (target_fd == -1) {
            if (errno != EACCES) {
                retval = NULL;
                goto EXIT;
            } else {
                continue;
            }
        }
        if (fstat(target_fd, &ts) == -1) {
            if (close(target_fd) == -1) {
                retval = NULL;
                goto EXIT;
            }
        }
        if (ts.st_ino == cs.st_ino &&
            ts.st_dev == cs.st_dev)
        {
            size_t ps, ns;
            ps = strlen(cwdbuf);
            ns = strlen(dp->d_name);
            if (ps + ns + 2 /* / + \0 */ > size) {
                errno = ERANGE;
                retval = NULL;
                goto EXIT;
            }
            strcpy(cwdbuf + ps, dp->d_name);
            if (level != 0) {
                cwdbuf[ps+ns] = '/';
                cwdbuf[ps+ns+1] = '\0';
            } else {
                cwdbuf[ps+ns] = '\0';
            }
        }
    }
    if (errno != 0) {
        retval = NULL;
        goto EXIT;
    }

EXIT:
    if (current != NULL) {
        closedir(current);
    }
    if (parent != NULL) {
        closedir(parent);
    }
    return retval;
}


static char *
m_getcwd(char *cwdbuf, size_t size)
{
    int ofd;
    char* cwd;

    if (size <= 0) {
        errno = ERANGE;
        return NULL;
    }

    ofd = open(".", O_RDONLY);
    if (ofd == -1) {
        return NULL;
    }

    cwdbuf[0] = '\0';
    cwd = m_getcwd_iter(cwdbuf, size, 0);

    if (fchdir(ofd) == -1) {
        return NULL;
    }
    if (close(ofd) == -1) {
        return NULL;
    }

    return cwd;
}
