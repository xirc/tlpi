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
#include <sys/wait.h>


void
mfork()
{
    pid_t ppid;
    pid_t cpid;

    ppid = getpid();
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (cpid == 0) {
        printf("%ld --> %ld\n", (long) ppid, (long) getpid());
    }
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    setbuf(stdout, NULL);
        /* Make stdout unbuffered */

    printf("%ld\n", (long) getpid());
    mfork();
    mfork();
    mfork();

    exit(EXIT_SUCCESS);
}
