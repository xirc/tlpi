/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static int
m_setenv(const char *name, const char *value, int overwrite)
{
    if (!overwrite && getenv(name) != NULL) {
        return 0;
    }

    char *buffer;
    size_t len, name_len;

    name_len = strlen(name);
    len = name_len + strlen(value) + 1;

    buffer = malloc(len);
    if (buffer == NULL) {
        return -1;
    }

    strcpy(buffer, name);
    buffer[name_len] = '=';
    strcpy(&buffer[name_len+1], value);

    putenv(buffer);
    return 0;
}


static int
m_unsetenv(const char *name)
{
    while (getenv(name) != NULL) {
        putenv((char*)name);
    }
    return 0;
}


extern char **environ;

int
main(int argc, char *argv[])
{

    int j;
    char **ep;

    /* erase entire environment */
    clearenv();

    /* set new environment */
    for (j = 1; j < argc; ++j) {
        if (putenv(argv[j]) != 0) {
            fprintf(stderr, "putenv: %s\n", argv[j]);
            exit(EXIT_FAILURE);
        }
    }

    if (m_setenv("GREET", "Hello world", 0) == -1) {
        fprintf(stderr, "setenv");
        exit(EXIT_FAILURE);
    }

    m_unsetenv("BYE");

    for (ep = environ; *ep != NULL; ep++) {
        puts(*ep);
    }

    exit(EXIT_SUCCESS);
}
