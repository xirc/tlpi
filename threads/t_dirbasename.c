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
#include <limits.h>
#include <pthread.h>
#include <libgen.h>
#include "m_dirbasename.h"


#define ASSERT(EXPR) \
    do { \
        if (!(EXPR)) { \
            fprintf(stderr, "ASSERTION ERROR " #EXPR "\n"); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)


static int use_my_dirbasename;


static char *
__basename__(char *path)
{
    if (use_my_dirbasename) {
        return m_basename(path);
    } else {
        return basename(path);
    }
}


static char *
__dirname__(char *path)
{
    if (use_my_dirbasename) {
        return m_dirname(path);
    } else {
        return dirname(path);
    }
}


static void
test_dirbasename(char const *path,
                 char const *expect_dir, char const *expect_base)
{
    char buf4dir[PATH_MAX];
    char buf4base[PATH_MAX];
    char *dir, *base;

    printf("path = %s (%p)\n", path, path);
    if (path != NULL) {
        strncpy(buf4dir, path, PATH_MAX);
        buf4dir[PATH_MAX-1] = '\0';
        strncpy(buf4base, path, PATH_MAX);
        buf4base[PATH_MAX-1] = '\0';
    }

    if (path != NULL) {
        dir = __dirname__(buf4dir);
        base = __basename__(buf4base);
    } else {
        dir = __dirname__(NULL);
        base = __basename__(NULL);
    }
    ASSERT(strcmp(dir, expect_dir) == 0);
    ASSERT(strcmp(base, expect_base) == 0);

    printf("    DIR: %s (%p), EXPECT: %s, PATH: %s (%p)\n",
            dir, dir, expect_dir,
            path != NULL ? buf4dir : NULL,
            path != NULL ? buf4dir : NULL);
    printf("    BASE: %s (%p), EXPECT: %s, PATH: %s (%p)\n",
            base, base, expect_base,
            path != NULL ? buf4base : NULL,
            path != NULL ? buf4base : NULL);
}


int
main(int argc, char *argv[])
{
    if (argc < 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [use-libgen]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    use_my_dirbasename = (argc > 1) ? 0 : 1;

    test_dirbasename("/etc/bin/zip", "/etc/bin", "zip");
    test_dirbasename("/etc/passwd////", "/etc", "passwd");
    test_dirbasename("/etc////passwd", "/etc", "passwd");
    test_dirbasename("etc/passwd", "etc", "passwd");
    test_dirbasename("passwd", ".", "passwd");
    test_dirbasename("passwd/", ".", "passwd");
    test_dirbasename("/", "/", "/");
    test_dirbasename("..", ".", "..");
    test_dirbasename(NULL, ".", ".");

    exit(EXIT_SUCCESS);
}
