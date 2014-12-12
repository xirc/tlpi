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
#include <stdarg.h>
#include <limits.h>
#include <string.h>


static int
m_execlp(const char *filename, const char *args, ...)
{
    va_list ap;
    char **argv, **tmp;
    size_t nargv;
    char *envv[1] = { NULL };
    unsigned int argc;
    int retc, errc;

    /* this value is for implement test (Actually, it should be more larger value) */
    nargv = 1;
    argv = malloc(sizeof(char*) * nargv);
    if (argv == NULL) {
        return -1;
    }

    va_start(ap, args);
    argc = 0;
    do {
        if (argc >= nargv) {
            nargv *= 2;
            tmp = realloc(argv, nargv);
            if (tmp == NULL) {
                return -1;
            }
            argv = tmp;
        }
        argv[argc++] = va_arg(ap, char*);
    } while (argv[argc-1] != NULL);
    va_end(ap);

    retc = execve(filename, argv, envv);
    errc = errno;
    free(argv);

    errno = errc;
    return retc;
}


int
main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s pathname\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    m_execlp(argv[1], argv[1], "hello world", "m_execlp", (char *) NULL);

    /* If we get here, something went wrong */
    perror("execlp");
    exit(EXIT_FAILURE);
}
