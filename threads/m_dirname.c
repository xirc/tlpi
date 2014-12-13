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
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>
#include "m_dirbasename.h"


static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t key;


static void
destructor(void *buf)
{
    free(buf);
}


static void
create_key(void)
{
    int s;

    s = pthread_key_create(&key, destructor);
    if (s != 0) {
        exit(EXIT_FAILURE);
    }
}


static void
regpath(char *str)
{
    size_t i, j, len;

    len = strlen(str);
    for (i = 0; i < len; ++i) {
        if (str[i] != '/') continue;
        for (j = i+1; j < len && str[j] == '/'; ++j);
        if (j > i+1) {
            memmove(str+i+1, str+j, len - j + 1);
            len -= j - i - 1;
        }
    }
    if (len > 1 && str[len-1] == '/') {
        str[len-1] = '\0';
    }
}


char *
m_dirname(char const *path)
{
    int s;
    char *buf, *p;

    s = pthread_once(&once, create_key);
    if (s != 0) {
        perror("basename_r:pthread_once");
        exit(EXIT_FAILURE);
    }

    buf = pthread_getspecific(key);
    if (buf == NULL) {
        buf = malloc(PATH_MAX);
        if (buf == NULL) {
            perror("basename_r:malloc");
            exit(EXIT_FAILURE);
        }

        s = pthread_setspecific(key, buf);
        if (s != 0) {
            perror("basename_r:pthread_setspecific");
            exit(EXIT_FAILURE);
        }
    }

    if (path == NULL) {
        return ".";
    }

    (void) strncpy(buf, path, PATH_MAX);
    buf[PATH_MAX] = '\0';
    regpath(buf);

    if (strcmp(buf, "/") == 0) {
        return "/";
    }

    p = strrchr(buf, '/');
    if (p == NULL) {
        return ".";
    } else  {
        buf[p - buf] = '\0';
    }
    return buf;
}
