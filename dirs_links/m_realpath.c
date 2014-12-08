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
#include <limits.h>
#include <string.h>
#include <libgen.h>


static char *
my_realpath(char const *pathname, char *resolved_path)
{
    char path[PATH_MAX];

    char *sp, *tok, *dir;
    size_t len, toklen;

    if (getcwd(resolved_path, PATH_MAX) == NULL) {
        return NULL;
    }

    strcpy(path, pathname);
    for (sp = path; (tok = strtok(sp, "/")) != NULL; sp = NULL) {
        if (strcmp(tok, ".") == 0) {
            /* do nothing */
        } else if (strcmp(tok, "..") == 0) {
            dir = dirname(resolved_path);
            if (dir == NULL) {
                return NULL;
            }
            memmove(resolved_path, dir, strlen(dir)+1);
        } else {
            len = strlen(resolved_path);
            toklen = strlen(tok);
            if (len + toklen + 2 /* / + NULL */ > PATH_MAX) {
                return NULL;
            }
            resolved_path[len] = '/';
            strcpy(resolved_path+len+1, tok);
        }
    }

    return resolved_path;
}


int
main(int argc, char *argv[])
{
    char buf[PATH_MAX];

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(buf, 0, PATH_MAX);
    if (realpath(argv[1], buf) == NULL) {
        perror("realpath");
        exit(EXIT_FAILURE);
    }
    printf("realpath:    %s --> %s\n", argv[1], buf);

    memset(buf, 0, PATH_MAX);
    if (my_realpath(argv[1], buf) == NULL) {
        perror("realpath");
        exit(EXIT_FAILURE);
    }
    printf("my_realpath: %s --> %s\n", argv[1], buf);

    exit(EXIT_FAILURE);
}
