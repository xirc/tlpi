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
#include <sys/resource.h>


int
main(int argc, char *argv[])
{
    int rc, is_pre_malloc;
    struct rlimit rlim;
    char *p;

    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [pre-malloc]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    is_pre_malloc = (argc >= 2);

    if (is_pre_malloc) {
        printf("malloc (512 + 256) * sizeof(char)\n");
        p = malloc((512 + 256) * sizeof(char));
        if (p == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }

    rlim.rlim_cur =  512; /*  512 bytes */
    rlim.rlim_max = 1024; /* 1024 bytes */
    printf("setrlimit(RLIMIT_AS, {soft=%lld, hard=%lld})\n",
            (long long) rlim.rlim_cur, (long long) rlim.rlim_max);
    rc = setrlimit(RLIMIT_AS, &rlim);
    if (rc == -1) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }

    rlim.rlim_cur = 0;
    rlim.rlim_max = 0;
    rc = getrlimit(RLIMIT_AS, &rlim);
    if (rc == -1) {
        perror("getrlimit");
        exit(EXIT_FAILURE);
    }
    printf("getrlimit(RLIMIT_AS,...) = { soft=%lld, hard=%lld}\n",
            (long long) rlim.rlim_cur, (long long) rlim.rlim_max);

    if (is_pre_malloc) {
        printf("free\n");
        free(p);
    }

    printf("malloc (512 + 256) * sizeof(char)\n");
    p = malloc((512 + 256) * sizeof(char));
    if (p == NULL && errno == ENOMEM) {
        printf("EXPECTS: errno = %d, errmsg = %s\n",
                errno, strerror(errno));
    } else {
        fprintf(stderr, "UNEXPECTED RESULTS (errno = %d, errmsg =%s)\n",
                errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
