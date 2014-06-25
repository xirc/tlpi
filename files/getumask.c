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


#define UMASK_SETTING (S_IWGRP | S_IXGRP | S_IWOTH | S_IXOTH)


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    mode_t u;

    /* set umask */
    umask(UMASK_SETTING);

    /* retrieve umask */
    umask(u = umask(0));
    printf("retrieve 1st : %03o\n", u);

    /* retrieve umask */
    umask(u = umask(0));
    printf("retrieve 2nd : %03o\n", u);

    exit(EXIT_SUCCESS);
}
