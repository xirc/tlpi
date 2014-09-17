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
#include <sys/resource.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int who;
    struct rusage res_usage;
    int retc;

    who = RUSAGE_SELF;

    retc = getrusage(who, &res_usage);
    if (retc == -1) {
        perror("getrusage");
        exit(EXIT_FAILURE);
    }

    printf("ru_utime: %ld s %ld us\n",
            (long) res_usage.ru_utime.tv_sec,
            (long) res_usage.ru_utime.tv_usec);
    printf("ru_stime: %ld s %ld us\n",
            (long) res_usage.ru_stime.tv_sec,
            (long) res_usage.ru_stime.tv_usec);
    printf("ru_maxrss: %ld [KB]\n",
            res_usage.ru_maxrss);
    printf("ru_ixrss: %ld [KB]\n",
            res_usage.ru_ixrss);
    printf("ru_idrss: %ld [KB]\n",
            res_usage.ru_idrss);
    printf("ru_isrss: %ld [KB]\n",
            res_usage.ru_isrss);
    printf("ru_minflt: %ld\n",
            res_usage.ru_minflt);
    printf("ru_majflt: %ld\n",
            res_usage.ru_majflt);
    printf("ru_nswap: %ld\n",
            res_usage.ru_nswap);
    printf("ru_inblock: %ld\n",
            res_usage.ru_inblock);
    printf("ru_oublock: %ld\n",
            res_usage.ru_oublock);
    printf("ru_msgsnd: %ld\n",
            res_usage.ru_msgsnd);
    printf("ru_msgrcv: %ld\n",
            res_usage.ru_msgrcv);
    printf("ru_nsignals: %ld\n",
            res_usage.ru_nsignals);
    printf("ru_nvcsw: %ld\n",
            res_usage.ru_nvcsw);
    printf("ru_nivcsw: %ld\n",
            res_usage.ru_nivcsw);

    exit(EXIT_SUCCESS);
}
