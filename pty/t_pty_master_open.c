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

#include "pty_master_open.h"

#define MAX_SNAME 1024


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int mfd;
    char slname[MAX_SNAME];

    mfd = pty_master_open(slname, MAX_SNAME);
    if (mfd == -1) {
        perror("pty_master_open");
        exit(EXIT_FAILURE);
    }

    printf("master pty: %d\n", mfd);
    printf("slave pty:  %s\n", slname);

    if (close(mfd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
